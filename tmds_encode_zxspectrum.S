#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"

.syntax unified
.cpu cortex-m0plus
.thumb

// Using the following:
//
// - A font stored as a 1bpp bitmap, with row 0 of each character stored in
//   one contiguous array, then row 1, etc, where each character is 8 bits
//   wide
//
// - A character buffer
//
// - A colour buffer for each of R, G, B (so 3 planes total), each buffer
//   storing a 2-bit foreground and background colour for each character
//
// Generate encoded TMDS buffers, fast enough to fit all the encode on one
// core, and with small memory footprint (so no framebuffer of any depth!) The
// main trick here is a LUT with an 8 bit index, composed of 4x1bpp pixels
// (the 4 LSBs of the index) and a 2x2-bit palette (the four MSBs of the
// index). Each LUT entry is 4 TMDS symbols, so 2 32-bit words, giving a total
// table size of 2 kB.

// Offsets suitable for ldr/str (must be <= 0x7c):
#define ACCUM0_OFFS     (SIO_INTERP0_ACCUM0_OFFSET     - SIO_INTERP0_ACCUM0_OFFSET)
#define ACCUM1_OFFS     (SIO_INTERP0_ACCUM1_OFFSET     - SIO_INTERP0_ACCUM0_OFFSET)
#define ACCUM1_ADD_OFFS (SIO_INTERP0_ACCUM1_ADD_OFFSET - SIO_INTERP0_ACCUM0_OFFSET)
#define PEEK0_OFFS      (SIO_INTERP0_PEEK_LANE0_OFFSET - SIO_INTERP0_ACCUM0_OFFSET)
#define PEEK1_OFFS      (SIO_INTERP0_PEEK_LANE1_OFFSET - SIO_INTERP0_ACCUM0_OFFSET)
#define PEEK2_OFFS      (SIO_INTERP0_PEEK_FULL_OFFSET  - SIO_INTERP0_ACCUM0_OFFSET)
#define INTERP1         (SIO_INTERP1_ACCUM0_OFFSET     - SIO_INTERP0_ACCUM0_OFFSET)

// r2 contains output buffer pointer
// r4 contains a byte of pixel data
// r5 contains TMDS LUT 
// r6-r7 are for scratch + pixels
//
// Mask off the 2 bits we need and multiply by 8
.macro do_crumb pix_shift_instr pix_shamt 
  \pix_shift_instr r6, r4, #\pix_shamt
  ldr r7, =24 // 00011000
  ands r6, r7
  add r6, r5
  ldmia r6, {r6, r7}
  stmia r2!, {r6-r7}
.endm


// Once in the loop:
// r0 contains character buffer pointer (only updated once per 8 chars)
// r1 contains a one nibble colour channel value
// r2 contains output buffer pointer
// r3 contains 
// r4-r7 are for scratch + pixels
// r9 contains the TMDS LUT base.

// we need the attribute to colour bits table

.macro do_char charbuf_offs
	ldrb r4, [r0, #\charbuf_offs]
  lsls r5, r1, #5
  add r5, r9
	do_crumb lsrs 3
	do_crumb lsrs 1
	do_crumb lsls 1
	do_crumb lsls 3
	
.endm


// r0 is character buffer
// r1 is colour buffer
// r2 is output TMDS buffer
// r3 is flash flag
// First stack argument is the font bitmap for this scanline.
// The second arg is the color channel 

.section .scratch_x.tmds_encode_font_2bpp, "ax"
.global tmds_encode_font_2bpp
.type tmds_encode_font_2bpp,%function
.thumb_func
tmds_encode_font_2bpp:

	push {r4-r7, lr}
	mov r4, r8
	mov r5, r9
	mov r6, r10
	push {r4-r6}

  ldr r7, =(32 * 8 * 2 * 2)
	add r7, r2
	mov ip, r7

  mov r8, r3        // save the flash mask in r8

	ldr r3, [sp, #32] // 8 words saved, so 32-byte offset to first stack argument
  lsls r3, #8       // * 256
	ldr r7, =attr_to_colour_chan_B
  adds r3, r7


	ldr r7, =palettised_1bpp_tables
	mov r9, r7

	mov r10, r1

	b 2f
1:
	mov r4, r10
	ldrb r1, [r4]      // Load the colour attribute
	mov r5, r8
	ands r1, r5
	adds r4, #1        // Increment the attribute pointer
	adds r1, r3        // Pointer to the entry in attr_to_colour_chan_X
	ldrb r1, [r1]      // Load the translated colour b0b1f1f0
	mov r10, r4
	do_char 0 
	adds r0, #1
2:
	cmp r2, ip
	blo 1b

	pop {r4-r6}
	mov r8, r4
	mov r9, r5
	mov r10, r6
	mov r0, r2         // Return the output buffer pointer
	pop {r4-r7, pc}

//
// BORDER
//
// r0 is width in characters
// r1 is colour channel
// r2 is output TMDS buffer
// r3 is the colour attribute
//
.section .scratch_x.tmds_encode_border, "ax"
.global tmds_encode_border
.type tmds_encode_border,%function
.thumb_func
tmds_encode_border:

	push {r4-r7, lr}
	mov r4, r8
	mov r5, r9
	mov r6, r10
	push {r4-r6}

	lsls r0, #5         // Work out the number of words (chars * 8 * 2 * 2)
	add r0, r2
	mov ip, r0 

  lsls r1, #8         // * 256
	ldr r7, =attr_to_colour_chan_B
  adds r1, r7
  adds r1, r3
  ldrb r1, [r1]       // Load the colour intensity nibble for the channel
	ldr r7, =palettised_1bpp_tables
	lsls r1, #5         // * 8 * 4
  adds r1, r7         // r1 now point as the LUT entry 
  ldr r7, =(8*3)
  adds r6, r1, r7     // move to the entry 1111
  ldmia r6, {r6, r7}  // Load the LUT entries into r6,r7
	b 4f
3:
  stmia r2!, {r6-r7}  // Write out the LUT entries 
4:
	cmp r2, ip
	blo 3b

	pop {r4-r6}
	mov r8, r4
	mov r9, r5
	mov r10, r6
	mov r0, r2         // Return the output buffer pointer
	pop {r4-r7, pc}


.section .scratch_x.palettised_1bpp_tables, "a"
.align 2
palettised_1bpp_tables:
	// background, foreground = 00, 00
	.word 0x7f103, 0x7f103 // 0000
	.word 0x7f103, 0x7f103 // 1100
	.word 0x7f103, 0x7f103 // 0011
	.word 0x7f103, 0x7f103 // 1111
	// background, foreground = 00, 01
	.word 0x7f103, 0x7f103 // 0000
	.word 0x7f103, 0x73d30 // 1100
	.word 0x73d30, 0x7f103 // 0011
	.word 0x73d30, 0x73d30 // 1111
	// background, foreground = 00, 10
	.word 0x7f103, 0x7f103 // 0000
	.word 0x7f103, 0xb3e30 // 1100
	.word 0xb3e30, 0x7f103 // 0011
	.word 0xb3e30, 0xb3e30 // 1111
	// background, foreground = 00, 11
	.word 0x7f103, 0x7f103 // 0000
	.word 0x7f103, 0xbf203 // 1100
	.word 0xbf203, 0x7f103 // 0011
	.word 0xbf203, 0xbf203 // 1111
	// background, foreground = 01, 00
	.word 0x73d30, 0x73d30 // 0000
	.word 0x73d30, 0x7f103 // 1100
	.word 0x7f103, 0x73d30 // 0011
	.word 0x7f103, 0x7f103 // 1111
	// background, foreground = 01, 01
	.word 0x73d30, 0x73d30 // 0000
	.word 0x73d30, 0x73d30 // 1100
	.word 0x73d30, 0x73d30 // 0011
	.word 0x73d30, 0x73d30 // 1111
	// background, foreground = 01, 10
	.word 0x73d30, 0x73d30 // 0000
	.word 0x73d30, 0xb3e30 // 1100
	.word 0xb3e30, 0x73d30 // 0011
	.word 0xb3e30, 0xb3e30 // 1111
	// background, foreground = 01, 11
	.word 0x73d30, 0x73d30 // 0000
	.word 0x73d30, 0xbf203 // 1100
	.word 0xbf203, 0x73d30 // 0011
	.word 0xbf203, 0xbf203 // 1111
	// background, foreground = 10, 00
	.word 0xb3e30, 0xb3e30 // 0000
	.word 0xb3e30, 0x7f103 // 1100
	.word 0x7f103, 0xb3e30 // 0011
	.word 0x7f103, 0x7f103 // 1111
	// background, foreground = 10, 01
	.word 0xb3e30, 0xb3e30 // 0000
	.word 0xb3e30, 0x73d30 // 1100
	.word 0x73d30, 0xb3e30 // 0011
	.word 0x73d30, 0x73d30 // 1111
	// background, foreground = 10, 10
	.word 0xb3e30, 0xb3e30 // 0000
	.word 0xb3e30, 0xb3e30 // 1100
	.word 0xb3e30, 0xb3e30 // 0011
	.word 0xb3e30, 0xb3e30 // 1111
	// background, foreground = 10, 11
	.word 0xb3e30, 0xb3e30 // 0000
	.word 0xb3e30, 0xbf203 // 1100
	.word 0xbf203, 0xb3e30 // 0011
	.word 0xbf203, 0xbf203 // 1111
	// background, foreground = 11, 00
	.word 0xbf203, 0xbf203 // 0000
	.word 0xbf203, 0x7f103 // 1100
	.word 0x7f103, 0xbf203 // 0011
	.word 0x7f103, 0x7f103 // 1111
	// background, foreground = 11, 01
	.word 0xbf203, 0xbf203 // 0000
	.word 0xbf203, 0x73d30 // 1100
	.word 0x73d30, 0xbf203 // 0011
	.word 0x73d30, 0x73d30 // 1111
	// background, foreground = 11, 10
	.word 0xbf203, 0xbf203 // 0000
	.word 0xbf203, 0xb3e30 // 1100
	.word 0xb3e30, 0xbf203 // 0011
	.word 0xb3e30, 0xb3e30 // 1111
	// background, foreground = 11, 11
	.word 0xbf203, 0xbf203 // 0000
	.word 0xbf203, 0xbf203 // 1100
	.word 0xbf203, 0xbf203 // 0011
	.word 0xbf203, 0xbf203 // 1111

.align 1
attr_to_colour_chan_B:
.byte 0x00 // B 00000000 -> 00000000
.byte 0x02 // B 00000001 -> 00000010
.byte 0x00 // B 00000010 -> 00000000
.byte 0x02 // B 00000011 -> 00000010
.byte 0x00 // B 00000100 -> 00000000
.byte 0x02 // B 00000101 -> 00000010
.byte 0x00 // B 00000110 -> 00000000
.byte 0x02 // B 00000111 -> 00000010
.byte 0x08 // B 00001000 -> 00001000
.byte 0x0a // B 00001001 -> 00001010
.byte 0x08 // B 00001010 -> 00001000
.byte 0x0a // B 00001011 -> 00001010
.byte 0x08 // B 00001100 -> 00001000
.byte 0x0a // B 00001101 -> 00001010
.byte 0x08 // B 00001110 -> 00001000
.byte 0x0a // B 00001111 -> 00001010
.byte 0x00 // B 00010000 -> 00000000
.byte 0x02 // B 00010001 -> 00000010
.byte 0x00 // B 00010010 -> 00000000
.byte 0x02 // B 00010011 -> 00000010
.byte 0x00 // B 00010100 -> 00000000
.byte 0x02 // B 00010101 -> 00000010
.byte 0x00 // B 00010110 -> 00000000
.byte 0x02 // B 00010111 -> 00000010
.byte 0x08 // B 00011000 -> 00001000
.byte 0x0a // B 00011001 -> 00001010
.byte 0x08 // B 00011010 -> 00001000
.byte 0x0a // B 00011011 -> 00001010
.byte 0x08 // B 00011100 -> 00001000
.byte 0x0a // B 00011101 -> 00001010
.byte 0x08 // B 00011110 -> 00001000
.byte 0x0a // B 00011111 -> 00001010
.byte 0x00 // B 00100000 -> 00000000
.byte 0x02 // B 00100001 -> 00000010
.byte 0x00 // B 00100010 -> 00000000
.byte 0x02 // B 00100011 -> 00000010
.byte 0x00 // B 00100100 -> 00000000
.byte 0x02 // B 00100101 -> 00000010
.byte 0x00 // B 00100110 -> 00000000
.byte 0x02 // B 00100111 -> 00000010
.byte 0x08 // B 00101000 -> 00001000
.byte 0x0a // B 00101001 -> 00001010
.byte 0x08 // B 00101010 -> 00001000
.byte 0x0a // B 00101011 -> 00001010
.byte 0x08 // B 00101100 -> 00001000
.byte 0x0a // B 00101101 -> 00001010
.byte 0x08 // B 00101110 -> 00001000
.byte 0x0a // B 00101111 -> 00001010
.byte 0x00 // B 00110000 -> 00000000
.byte 0x02 // B 00110001 -> 00000010
.byte 0x00 // B 00110010 -> 00000000
.byte 0x02 // B 00110011 -> 00000010
.byte 0x00 // B 00110100 -> 00000000
.byte 0x02 // B 00110101 -> 00000010
.byte 0x00 // B 00110110 -> 00000000
.byte 0x02 // B 00110111 -> 00000010
.byte 0x08 // B 00111000 -> 00001000
.byte 0x0a // B 00111001 -> 00001010
.byte 0x08 // B 00111010 -> 00001000
.byte 0x0a // B 00111011 -> 00001010
.byte 0x08 // B 00111100 -> 00001000
.byte 0x0a // B 00111101 -> 00001010
.byte 0x08 // B 00111110 -> 00001000
.byte 0x0a // B 00111111 -> 00001010
.byte 0x05 // B 01000000 -> 00000101
.byte 0x07 // B 01000001 -> 00000111
.byte 0x05 // B 01000010 -> 00000101
.byte 0x07 // B 01000011 -> 00000111
.byte 0x05 // B 01000100 -> 00000101
.byte 0x07 // B 01000101 -> 00000111
.byte 0x05 // B 01000110 -> 00000101
.byte 0x07 // B 01000111 -> 00000111
.byte 0x0d // B 01001000 -> 00001101
.byte 0x0f // B 01001001 -> 00001111
.byte 0x0d // B 01001010 -> 00001101
.byte 0x0f // B 01001011 -> 00001111
.byte 0x0d // B 01001100 -> 00001101
.byte 0x0f // B 01001101 -> 00001111
.byte 0x0d // B 01001110 -> 00001101
.byte 0x0f // B 01001111 -> 00001111
.byte 0x05 // B 01010000 -> 00000101
.byte 0x07 // B 01010001 -> 00000111
.byte 0x05 // B 01010010 -> 00000101
.byte 0x07 // B 01010011 -> 00000111
.byte 0x05 // B 01010100 -> 00000101
.byte 0x07 // B 01010101 -> 00000111
.byte 0x05 // B 01010110 -> 00000101
.byte 0x07 // B 01010111 -> 00000111
.byte 0x0d // B 01011000 -> 00001101
.byte 0x0f // B 01011001 -> 00001111
.byte 0x0d // B 01011010 -> 00001101
.byte 0x0f // B 01011011 -> 00001111
.byte 0x0d // B 01011100 -> 00001101
.byte 0x0f // B 01011101 -> 00001111
.byte 0x0d // B 01011110 -> 00001101
.byte 0x0f // B 01011111 -> 00001111
.byte 0x05 // B 01100000 -> 00000101
.byte 0x07 // B 01100001 -> 00000111
.byte 0x05 // B 01100010 -> 00000101
.byte 0x07 // B 01100011 -> 00000111
.byte 0x05 // B 01100100 -> 00000101
.byte 0x07 // B 01100101 -> 00000111
.byte 0x05 // B 01100110 -> 00000101
.byte 0x07 // B 01100111 -> 00000111
.byte 0x0d // B 01101000 -> 00001101
.byte 0x0f // B 01101001 -> 00001111
.byte 0x0d // B 01101010 -> 00001101
.byte 0x0f // B 01101011 -> 00001111
.byte 0x0d // B 01101100 -> 00001101
.byte 0x0f // B 01101101 -> 00001111
.byte 0x0d // B 01101110 -> 00001101
.byte 0x0f // B 01101111 -> 00001111
.byte 0x05 // B 01110000 -> 00000101
.byte 0x07 // B 01110001 -> 00000111
.byte 0x05 // B 01110010 -> 00000101
.byte 0x07 // B 01110011 -> 00000111
.byte 0x05 // B 01110100 -> 00000101
.byte 0x07 // B 01110101 -> 00000111
.byte 0x05 // B 01110110 -> 00000101
.byte 0x07 // B 01110111 -> 00000111
.byte 0x0d // B 01111000 -> 00001101
.byte 0x0f // B 01111001 -> 00001111
.byte 0x0d // B 01111010 -> 00001101
.byte 0x0f // B 01111011 -> 00001111
.byte 0x0d // B 01111100 -> 00001101
.byte 0x0f // B 01111101 -> 00001111
.byte 0x0d // B 01111110 -> 00001101
.byte 0x0f // B 01111111 -> 00001111
.byte 0x00 // B 10000000 -> 00000000
.byte 0x08 // B 10000001 -> 00001000
.byte 0x00 // B 10000010 -> 00000000
.byte 0x08 // B 10000011 -> 00001000
.byte 0x00 // B 10000100 -> 00000000
.byte 0x08 // B 10000101 -> 00001000
.byte 0x00 // B 10000110 -> 00000000
.byte 0x08 // B 10000111 -> 00001000
.byte 0x02 // B 10001000 -> 00000010
.byte 0x0a // B 10001001 -> 00001010
.byte 0x02 // B 10001010 -> 00000010
.byte 0x0a // B 10001011 -> 00001010
.byte 0x02 // B 10001100 -> 00000010
.byte 0x0a // B 10001101 -> 00001010
.byte 0x02 // B 10001110 -> 00000010
.byte 0x0a // B 10001111 -> 00001010
.byte 0x00 // B 10010000 -> 00000000
.byte 0x08 // B 10010001 -> 00001000
.byte 0x00 // B 10010010 -> 00000000
.byte 0x08 // B 10010011 -> 00001000
.byte 0x00 // B 10010100 -> 00000000
.byte 0x08 // B 10010101 -> 00001000
.byte 0x00 // B 10010110 -> 00000000
.byte 0x08 // B 10010111 -> 00001000
.byte 0x02 // B 10011000 -> 00000010
.byte 0x0a // B 10011001 -> 00001010
.byte 0x02 // B 10011010 -> 00000010
.byte 0x0a // B 10011011 -> 00001010
.byte 0x02 // B 10011100 -> 00000010
.byte 0x0a // B 10011101 -> 00001010
.byte 0x02 // B 10011110 -> 00000010
.byte 0x0a // B 10011111 -> 00001010
.byte 0x00 // B 10100000 -> 00000000
.byte 0x08 // B 10100001 -> 00001000
.byte 0x00 // B 10100010 -> 00000000
.byte 0x08 // B 10100011 -> 00001000
.byte 0x00 // B 10100100 -> 00000000
.byte 0x08 // B 10100101 -> 00001000
.byte 0x00 // B 10100110 -> 00000000
.byte 0x08 // B 10100111 -> 00001000
.byte 0x02 // B 10101000 -> 00000010
.byte 0x0a // B 10101001 -> 00001010
.byte 0x02 // B 10101010 -> 00000010
.byte 0x0a // B 10101011 -> 00001010
.byte 0x02 // B 10101100 -> 00000010
.byte 0x0a // B 10101101 -> 00001010
.byte 0x02 // B 10101110 -> 00000010
.byte 0x0a // B 10101111 -> 00001010
.byte 0x00 // B 10110000 -> 00000000
.byte 0x08 // B 10110001 -> 00001000
.byte 0x00 // B 10110010 -> 00000000
.byte 0x08 // B 10110011 -> 00001000
.byte 0x00 // B 10110100 -> 00000000
.byte 0x08 // B 10110101 -> 00001000
.byte 0x00 // B 10110110 -> 00000000
.byte 0x08 // B 10110111 -> 00001000
.byte 0x02 // B 10111000 -> 00000010
.byte 0x0a // B 10111001 -> 00001010
.byte 0x02 // B 10111010 -> 00000010
.byte 0x0a // B 10111011 -> 00001010
.byte 0x02 // B 10111100 -> 00000010
.byte 0x0a // B 10111101 -> 00001010
.byte 0x02 // B 10111110 -> 00000010
.byte 0x0a // B 10111111 -> 00001010
.byte 0x05 // B 11000000 -> 00000101
.byte 0x0d // B 11000001 -> 00001101
.byte 0x05 // B 11000010 -> 00000101
.byte 0x0d // B 11000011 -> 00001101
.byte 0x05 // B 11000100 -> 00000101
.byte 0x0d // B 11000101 -> 00001101
.byte 0x05 // B 11000110 -> 00000101
.byte 0x0d // B 11000111 -> 00001101
.byte 0x07 // B 11001000 -> 00000111
.byte 0x0f // B 11001001 -> 00001111
.byte 0x07 // B 11001010 -> 00000111
.byte 0x0f // B 11001011 -> 00001111
.byte 0x07 // B 11001100 -> 00000111
.byte 0x0f // B 11001101 -> 00001111
.byte 0x07 // B 11001110 -> 00000111
.byte 0x0f // B 11001111 -> 00001111
.byte 0x05 // B 11010000 -> 00000101
.byte 0x0d // B 11010001 -> 00001101
.byte 0x05 // B 11010010 -> 00000101
.byte 0x0d // B 11010011 -> 00001101
.byte 0x05 // B 11010100 -> 00000101
.byte 0x0d // B 11010101 -> 00001101
.byte 0x05 // B 11010110 -> 00000101
.byte 0x0d // B 11010111 -> 00001101
.byte 0x07 // B 11011000 -> 00000111
.byte 0x0f // B 11011001 -> 00001111
.byte 0x07 // B 11011010 -> 00000111
.byte 0x0f // B 11011011 -> 00001111
.byte 0x07 // B 11011100 -> 00000111
.byte 0x0f // B 11011101 -> 00001111
.byte 0x07 // B 11011110 -> 00000111
.byte 0x0f // B 11011111 -> 00001111
.byte 0x05 // B 11100000 -> 00000101
.byte 0x0d // B 11100001 -> 00001101
.byte 0x05 // B 11100010 -> 00000101
.byte 0x0d // B 11100011 -> 00001101
.byte 0x05 // B 11100100 -> 00000101
.byte 0x0d // B 11100101 -> 00001101
.byte 0x05 // B 11100110 -> 00000101
.byte 0x0d // B 11100111 -> 00001101
.byte 0x07 // B 11101000 -> 00000111
.byte 0x0f // B 11101001 -> 00001111
.byte 0x07 // B 11101010 -> 00000111
.byte 0x0f // B 11101011 -> 00001111
.byte 0x07 // B 11101100 -> 00000111
.byte 0x0f // B 11101101 -> 00001111
.byte 0x07 // B 11101110 -> 00000111
.byte 0x0f // B 11101111 -> 00001111
.byte 0x05 // B 11110000 -> 00000101
.byte 0x0d // B 11110001 -> 00001101
.byte 0x05 // B 11110010 -> 00000101
.byte 0x0d // B 11110011 -> 00001101
.byte 0x05 // B 11110100 -> 00000101
.byte 0x0d // B 11110101 -> 00001101
.byte 0x05 // B 11110110 -> 00000101
.byte 0x0d // B 11110111 -> 00001101
.byte 0x07 // B 11111000 -> 00000111
.byte 0x0f // B 11111001 -> 00001111
.byte 0x07 // B 11111010 -> 00000111
.byte 0x0f // B 11111011 -> 00001111
.byte 0x07 // B 11111100 -> 00000111
.byte 0x0f // B 11111101 -> 00001111
.byte 0x07 // B 11111110 -> 00000111
.byte 0x0f // B 11111111 -> 00001111
.align 1
attr_to_colour_chan_G:
.byte 0x00 // G 00000000 -> 00000000
.byte 0x00 // G 00000001 -> 00000000
.byte 0x00 // G 00000010 -> 00000000
.byte 0x00 // G 00000011 -> 00000000
.byte 0x02 // G 00000100 -> 00000010
.byte 0x02 // G 00000101 -> 00000010
.byte 0x02 // G 00000110 -> 00000010
.byte 0x02 // G 00000111 -> 00000010
.byte 0x00 // G 00001000 -> 00000000
.byte 0x00 // G 00001001 -> 00000000
.byte 0x00 // G 00001010 -> 00000000
.byte 0x00 // G 00001011 -> 00000000
.byte 0x02 // G 00001100 -> 00000010
.byte 0x02 // G 00001101 -> 00000010
.byte 0x02 // G 00001110 -> 00000010
.byte 0x02 // G 00001111 -> 00000010
.byte 0x00 // G 00010000 -> 00000000
.byte 0x00 // G 00010001 -> 00000000
.byte 0x00 // G 00010010 -> 00000000
.byte 0x00 // G 00010011 -> 00000000
.byte 0x02 // G 00010100 -> 00000010
.byte 0x02 // G 00010101 -> 00000010
.byte 0x02 // G 00010110 -> 00000010
.byte 0x02 // G 00010111 -> 00000010
.byte 0x00 // G 00011000 -> 00000000
.byte 0x00 // G 00011001 -> 00000000
.byte 0x00 // G 00011010 -> 00000000
.byte 0x00 // G 00011011 -> 00000000
.byte 0x02 // G 00011100 -> 00000010
.byte 0x02 // G 00011101 -> 00000010
.byte 0x02 // G 00011110 -> 00000010
.byte 0x02 // G 00011111 -> 00000010
.byte 0x08 // G 00100000 -> 00001000
.byte 0x08 // G 00100001 -> 00001000
.byte 0x08 // G 00100010 -> 00001000
.byte 0x08 // G 00100011 -> 00001000
.byte 0x0a // G 00100100 -> 00001010
.byte 0x0a // G 00100101 -> 00001010
.byte 0x0a // G 00100110 -> 00001010
.byte 0x0a // G 00100111 -> 00001010
.byte 0x08 // G 00101000 -> 00001000
.byte 0x08 // G 00101001 -> 00001000
.byte 0x08 // G 00101010 -> 00001000
.byte 0x08 // G 00101011 -> 00001000
.byte 0x0a // G 00101100 -> 00001010
.byte 0x0a // G 00101101 -> 00001010
.byte 0x0a // G 00101110 -> 00001010
.byte 0x0a // G 00101111 -> 00001010
.byte 0x08 // G 00110000 -> 00001000
.byte 0x08 // G 00110001 -> 00001000
.byte 0x08 // G 00110010 -> 00001000
.byte 0x08 // G 00110011 -> 00001000
.byte 0x0a // G 00110100 -> 00001010
.byte 0x0a // G 00110101 -> 00001010
.byte 0x0a // G 00110110 -> 00001010
.byte 0x0a // G 00110111 -> 00001010
.byte 0x08 // G 00111000 -> 00001000
.byte 0x08 // G 00111001 -> 00001000
.byte 0x08 // G 00111010 -> 00001000
.byte 0x08 // G 00111011 -> 00001000
.byte 0x0a // G 00111100 -> 00001010
.byte 0x0a // G 00111101 -> 00001010
.byte 0x0a // G 00111110 -> 00001010
.byte 0x0a // G 00111111 -> 00001010
.byte 0x05 // G 01000000 -> 00000101
.byte 0x05 // G 01000001 -> 00000101
.byte 0x05 // G 01000010 -> 00000101
.byte 0x05 // G 01000011 -> 00000101
.byte 0x07 // G 01000100 -> 00000111
.byte 0x07 // G 01000101 -> 00000111
.byte 0x07 // G 01000110 -> 00000111
.byte 0x07 // G 01000111 -> 00000111
.byte 0x05 // G 01001000 -> 00000101
.byte 0x05 // G 01001001 -> 00000101
.byte 0x05 // G 01001010 -> 00000101
.byte 0x05 // G 01001011 -> 00000101
.byte 0x07 // G 01001100 -> 00000111
.byte 0x07 // G 01001101 -> 00000111
.byte 0x07 // G 01001110 -> 00000111
.byte 0x07 // G 01001111 -> 00000111
.byte 0x05 // G 01010000 -> 00000101
.byte 0x05 // G 01010001 -> 00000101
.byte 0x05 // G 01010010 -> 00000101
.byte 0x05 // G 01010011 -> 00000101
.byte 0x07 // G 01010100 -> 00000111
.byte 0x07 // G 01010101 -> 00000111
.byte 0x07 // G 01010110 -> 00000111
.byte 0x07 // G 01010111 -> 00000111
.byte 0x05 // G 01011000 -> 00000101
.byte 0x05 // G 01011001 -> 00000101
.byte 0x05 // G 01011010 -> 00000101
.byte 0x05 // G 01011011 -> 00000101
.byte 0x07 // G 01011100 -> 00000111
.byte 0x07 // G 01011101 -> 00000111
.byte 0x07 // G 01011110 -> 00000111
.byte 0x07 // G 01011111 -> 00000111
.byte 0x0d // G 01100000 -> 00001101
.byte 0x0d // G 01100001 -> 00001101
.byte 0x0d // G 01100010 -> 00001101
.byte 0x0d // G 01100011 -> 00001101
.byte 0x0f // G 01100100 -> 00001111
.byte 0x0f // G 01100101 -> 00001111
.byte 0x0f // G 01100110 -> 00001111
.byte 0x0f // G 01100111 -> 00001111
.byte 0x0d // G 01101000 -> 00001101
.byte 0x0d // G 01101001 -> 00001101
.byte 0x0d // G 01101010 -> 00001101
.byte 0x0d // G 01101011 -> 00001101
.byte 0x0f // G 01101100 -> 00001111
.byte 0x0f // G 01101101 -> 00001111
.byte 0x0f // G 01101110 -> 00001111
.byte 0x0f // G 01101111 -> 00001111
.byte 0x0d // G 01110000 -> 00001101
.byte 0x0d // G 01110001 -> 00001101
.byte 0x0d // G 01110010 -> 00001101
.byte 0x0d // G 01110011 -> 00001101
.byte 0x0f // G 01110100 -> 00001111
.byte 0x0f // G 01110101 -> 00001111
.byte 0x0f // G 01110110 -> 00001111
.byte 0x0f // G 01110111 -> 00001111
.byte 0x0d // G 01111000 -> 00001101
.byte 0x0d // G 01111001 -> 00001101
.byte 0x0d // G 01111010 -> 00001101
.byte 0x0d // G 01111011 -> 00001101
.byte 0x0f // G 01111100 -> 00001111
.byte 0x0f // G 01111101 -> 00001111
.byte 0x0f // G 01111110 -> 00001111
.byte 0x0f // G 01111111 -> 00001111
.byte 0x00 // G 10000000 -> 00000000
.byte 0x00 // G 10000001 -> 00000000
.byte 0x00 // G 10000010 -> 00000000
.byte 0x00 // G 10000011 -> 00000000
.byte 0x08 // G 10000100 -> 00001000
.byte 0x08 // G 10000101 -> 00001000
.byte 0x08 // G 10000110 -> 00001000
.byte 0x08 // G 10000111 -> 00001000
.byte 0x00 // G 10001000 -> 00000000
.byte 0x00 // G 10001001 -> 00000000
.byte 0x00 // G 10001010 -> 00000000
.byte 0x00 // G 10001011 -> 00000000
.byte 0x08 // G 10001100 -> 00001000
.byte 0x08 // G 10001101 -> 00001000
.byte 0x08 // G 10001110 -> 00001000
.byte 0x08 // G 10001111 -> 00001000
.byte 0x00 // G 10010000 -> 00000000
.byte 0x00 // G 10010001 -> 00000000
.byte 0x00 // G 10010010 -> 00000000
.byte 0x00 // G 10010011 -> 00000000
.byte 0x08 // G 10010100 -> 00001000
.byte 0x08 // G 10010101 -> 00001000
.byte 0x08 // G 10010110 -> 00001000
.byte 0x08 // G 10010111 -> 00001000
.byte 0x00 // G 10011000 -> 00000000
.byte 0x00 // G 10011001 -> 00000000
.byte 0x00 // G 10011010 -> 00000000
.byte 0x00 // G 10011011 -> 00000000
.byte 0x08 // G 10011100 -> 00001000
.byte 0x08 // G 10011101 -> 00001000
.byte 0x08 // G 10011110 -> 00001000
.byte 0x08 // G 10011111 -> 00001000
.byte 0x02 // G 10100000 -> 00000010
.byte 0x02 // G 10100001 -> 00000010
.byte 0x02 // G 10100010 -> 00000010
.byte 0x02 // G 10100011 -> 00000010
.byte 0x0a // G 10100100 -> 00001010
.byte 0x0a // G 10100101 -> 00001010
.byte 0x0a // G 10100110 -> 00001010
.byte 0x0a // G 10100111 -> 00001010
.byte 0x02 // G 10101000 -> 00000010
.byte 0x02 // G 10101001 -> 00000010
.byte 0x02 // G 10101010 -> 00000010
.byte 0x02 // G 10101011 -> 00000010
.byte 0x0a // G 10101100 -> 00001010
.byte 0x0a // G 10101101 -> 00001010
.byte 0x0a // G 10101110 -> 00001010
.byte 0x0a // G 10101111 -> 00001010
.byte 0x02 // G 10110000 -> 00000010
.byte 0x02 // G 10110001 -> 00000010
.byte 0x02 // G 10110010 -> 00000010
.byte 0x02 // G 10110011 -> 00000010
.byte 0x0a // G 10110100 -> 00001010
.byte 0x0a // G 10110101 -> 00001010
.byte 0x0a // G 10110110 -> 00001010
.byte 0x0a // G 10110111 -> 00001010
.byte 0x02 // G 10111000 -> 00000010
.byte 0x02 // G 10111001 -> 00000010
.byte 0x02 // G 10111010 -> 00000010
.byte 0x02 // G 10111011 -> 00000010
.byte 0x0a // G 10111100 -> 00001010
.byte 0x0a // G 10111101 -> 00001010
.byte 0x0a // G 10111110 -> 00001010
.byte 0x0a // G 10111111 -> 00001010
.byte 0x05 // G 11000000 -> 00000101
.byte 0x05 // G 11000001 -> 00000101
.byte 0x05 // G 11000010 -> 00000101
.byte 0x05 // G 11000011 -> 00000101
.byte 0x0d // G 11000100 -> 00001101
.byte 0x0d // G 11000101 -> 00001101
.byte 0x0d // G 11000110 -> 00001101
.byte 0x0d // G 11000111 -> 00001101
.byte 0x05 // G 11001000 -> 00000101
.byte 0x05 // G 11001001 -> 00000101
.byte 0x05 // G 11001010 -> 00000101
.byte 0x05 // G 11001011 -> 00000101
.byte 0x0d // G 11001100 -> 00001101
.byte 0x0d // G 11001101 -> 00001101
.byte 0x0d // G 11001110 -> 00001101
.byte 0x0d // G 11001111 -> 00001101
.byte 0x05 // G 11010000 -> 00000101
.byte 0x05 // G 11010001 -> 00000101
.byte 0x05 // G 11010010 -> 00000101
.byte 0x05 // G 11010011 -> 00000101
.byte 0x0d // G 11010100 -> 00001101
.byte 0x0d // G 11010101 -> 00001101
.byte 0x0d // G 11010110 -> 00001101
.byte 0x0d // G 11010111 -> 00001101
.byte 0x05 // G 11011000 -> 00000101
.byte 0x05 // G 11011001 -> 00000101
.byte 0x05 // G 11011010 -> 00000101
.byte 0x05 // G 11011011 -> 00000101
.byte 0x0d // G 11011100 -> 00001101
.byte 0x0d // G 11011101 -> 00001101
.byte 0x0d // G 11011110 -> 00001101
.byte 0x0d // G 11011111 -> 00001101
.byte 0x07 // G 11100000 -> 00000111
.byte 0x07 // G 11100001 -> 00000111
.byte 0x07 // G 11100010 -> 00000111
.byte 0x07 // G 11100011 -> 00000111
.byte 0x0f // G 11100100 -> 00001111
.byte 0x0f // G 11100101 -> 00001111
.byte 0x0f // G 11100110 -> 00001111
.byte 0x0f // G 11100111 -> 00001111
.byte 0x07 // G 11101000 -> 00000111
.byte 0x07 // G 11101001 -> 00000111
.byte 0x07 // G 11101010 -> 00000111
.byte 0x07 // G 11101011 -> 00000111
.byte 0x0f // G 11101100 -> 00001111
.byte 0x0f // G 11101101 -> 00001111
.byte 0x0f // G 11101110 -> 00001111
.byte 0x0f // G 11101111 -> 00001111
.byte 0x07 // G 11110000 -> 00000111
.byte 0x07 // G 11110001 -> 00000111
.byte 0x07 // G 11110010 -> 00000111
.byte 0x07 // G 11110011 -> 00000111
.byte 0x0f // G 11110100 -> 00001111
.byte 0x0f // G 11110101 -> 00001111
.byte 0x0f // G 11110110 -> 00001111
.byte 0x0f // G 11110111 -> 00001111
.byte 0x07 // G 11111000 -> 00000111
.byte 0x07 // G 11111001 -> 00000111
.byte 0x07 // G 11111010 -> 00000111
.byte 0x07 // G 11111011 -> 00000111
.byte 0x0f // G 11111100 -> 00001111
.byte 0x0f // G 11111101 -> 00001111
.byte 0x0f // G 11111110 -> 00001111
.byte 0x0f // G 11111111 -> 00001111
.align 1
attr_to_colour_chan_R:
.byte 0x00 // R 00000000 -> 00000000
.byte 0x00 // R 00000001 -> 00000000
.byte 0x02 // R 00000010 -> 00000010
.byte 0x02 // R 00000011 -> 00000010
.byte 0x00 // R 00000100 -> 00000000
.byte 0x00 // R 00000101 -> 00000000
.byte 0x02 // R 00000110 -> 00000010
.byte 0x02 // R 00000111 -> 00000010
.byte 0x00 // R 00001000 -> 00000000
.byte 0x00 // R 00001001 -> 00000000
.byte 0x02 // R 00001010 -> 00000010
.byte 0x02 // R 00001011 -> 00000010
.byte 0x00 // R 00001100 -> 00000000
.byte 0x00 // R 00001101 -> 00000000
.byte 0x02 // R 00001110 -> 00000010
.byte 0x02 // R 00001111 -> 00000010
.byte 0x08 // R 00010000 -> 00001000
.byte 0x08 // R 00010001 -> 00001000
.byte 0x0a // R 00010010 -> 00001010
.byte 0x0a // R 00010011 -> 00001010
.byte 0x08 // R 00010100 -> 00001000
.byte 0x08 // R 00010101 -> 00001000
.byte 0x0a // R 00010110 -> 00001010
.byte 0x0a // R 00010111 -> 00001010
.byte 0x08 // R 00011000 -> 00001000
.byte 0x08 // R 00011001 -> 00001000
.byte 0x0a // R 00011010 -> 00001010
.byte 0x0a // R 00011011 -> 00001010
.byte 0x08 // R 00011100 -> 00001000
.byte 0x08 // R 00011101 -> 00001000
.byte 0x0a // R 00011110 -> 00001010
.byte 0x0a // R 00011111 -> 00001010
.byte 0x00 // R 00100000 -> 00000000
.byte 0x00 // R 00100001 -> 00000000
.byte 0x02 // R 00100010 -> 00000010
.byte 0x02 // R 00100011 -> 00000010
.byte 0x00 // R 00100100 -> 00000000
.byte 0x00 // R 00100101 -> 00000000
.byte 0x02 // R 00100110 -> 00000010
.byte 0x02 // R 00100111 -> 00000010
.byte 0x00 // R 00101000 -> 00000000
.byte 0x00 // R 00101001 -> 00000000
.byte 0x02 // R 00101010 -> 00000010
.byte 0x02 // R 00101011 -> 00000010
.byte 0x00 // R 00101100 -> 00000000
.byte 0x00 // R 00101101 -> 00000000
.byte 0x02 // R 00101110 -> 00000010
.byte 0x02 // R 00101111 -> 00000010
.byte 0x08 // R 00110000 -> 00001000
.byte 0x08 // R 00110001 -> 00001000
.byte 0x0a // R 00110010 -> 00001010
.byte 0x0a // R 00110011 -> 00001010
.byte 0x08 // R 00110100 -> 00001000
.byte 0x08 // R 00110101 -> 00001000
.byte 0x0a // R 00110110 -> 00001010
.byte 0x0a // R 00110111 -> 00001010
.byte 0x08 // R 00111000 -> 00001000
.byte 0x08 // R 00111001 -> 00001000
.byte 0x0a // R 00111010 -> 00001010
.byte 0x0a // R 00111011 -> 00001010
.byte 0x08 // R 00111100 -> 00001000
.byte 0x08 // R 00111101 -> 00001000
.byte 0x0a // R 00111110 -> 00001010
.byte 0x0a // R 00111111 -> 00001010
.byte 0x05 // R 01000000 -> 00000101
.byte 0x05 // R 01000001 -> 00000101
.byte 0x07 // R 01000010 -> 00000111
.byte 0x07 // R 01000011 -> 00000111
.byte 0x05 // R 01000100 -> 00000101
.byte 0x05 // R 01000101 -> 00000101
.byte 0x07 // R 01000110 -> 00000111
.byte 0x07 // R 01000111 -> 00000111
.byte 0x05 // R 01001000 -> 00000101
.byte 0x05 // R 01001001 -> 00000101
.byte 0x07 // R 01001010 -> 00000111
.byte 0x07 // R 01001011 -> 00000111
.byte 0x05 // R 01001100 -> 00000101
.byte 0x05 // R 01001101 -> 00000101
.byte 0x07 // R 01001110 -> 00000111
.byte 0x07 // R 01001111 -> 00000111
.byte 0x0d // R 01010000 -> 00001101
.byte 0x0d // R 01010001 -> 00001101
.byte 0x0f // R 01010010 -> 00001111
.byte 0x0f // R 01010011 -> 00001111
.byte 0x0d // R 01010100 -> 00001101
.byte 0x0d // R 01010101 -> 00001101
.byte 0x0f // R 01010110 -> 00001111
.byte 0x0f // R 01010111 -> 00001111
.byte 0x0d // R 01011000 -> 00001101
.byte 0x0d // R 01011001 -> 00001101
.byte 0x0f // R 01011010 -> 00001111
.byte 0x0f // R 01011011 -> 00001111
.byte 0x0d // R 01011100 -> 00001101
.byte 0x0d // R 01011101 -> 00001101
.byte 0x0f // R 01011110 -> 00001111
.byte 0x0f // R 01011111 -> 00001111
.byte 0x05 // R 01100000 -> 00000101
.byte 0x05 // R 01100001 -> 00000101
.byte 0x07 // R 01100010 -> 00000111
.byte 0x07 // R 01100011 -> 00000111
.byte 0x05 // R 01100100 -> 00000101
.byte 0x05 // R 01100101 -> 00000101
.byte 0x07 // R 01100110 -> 00000111
.byte 0x07 // R 01100111 -> 00000111
.byte 0x05 // R 01101000 -> 00000101
.byte 0x05 // R 01101001 -> 00000101
.byte 0x07 // R 01101010 -> 00000111
.byte 0x07 // R 01101011 -> 00000111
.byte 0x05 // R 01101100 -> 00000101
.byte 0x05 // R 01101101 -> 00000101
.byte 0x07 // R 01101110 -> 00000111
.byte 0x07 // R 01101111 -> 00000111
.byte 0x0d // R 01110000 -> 00001101
.byte 0x0d // R 01110001 -> 00001101
.byte 0x0f // R 01110010 -> 00001111
.byte 0x0f // R 01110011 -> 00001111
.byte 0x0d // R 01110100 -> 00001101
.byte 0x0d // R 01110101 -> 00001101
.byte 0x0f // R 01110110 -> 00001111
.byte 0x0f // R 01110111 -> 00001111
.byte 0x0d // R 01111000 -> 00001101
.byte 0x0d // R 01111001 -> 00001101
.byte 0x0f // R 01111010 -> 00001111
.byte 0x0f // R 01111011 -> 00001111
.byte 0x0d // R 01111100 -> 00001101
.byte 0x0d // R 01111101 -> 00001101
.byte 0x0f // R 01111110 -> 00001111
.byte 0x0f // R 01111111 -> 00001111
.byte 0x00 // R 10000000 -> 00000000
.byte 0x00 // R 10000001 -> 00000000
.byte 0x08 // R 10000010 -> 00001000
.byte 0x08 // R 10000011 -> 00001000
.byte 0x00 // R 10000100 -> 00000000
.byte 0x00 // R 10000101 -> 00000000
.byte 0x08 // R 10000110 -> 00001000
.byte 0x08 // R 10000111 -> 00001000
.byte 0x00 // R 10001000 -> 00000000
.byte 0x00 // R 10001001 -> 00000000
.byte 0x08 // R 10001010 -> 00001000
.byte 0x08 // R 10001011 -> 00001000
.byte 0x00 // R 10001100 -> 00000000
.byte 0x00 // R 10001101 -> 00000000
.byte 0x08 // R 10001110 -> 00001000
.byte 0x08 // R 10001111 -> 00001000
.byte 0x02 // R 10010000 -> 00000010
.byte 0x02 // R 10010001 -> 00000010
.byte 0x0a // R 10010010 -> 00001010
.byte 0x0a // R 10010011 -> 00001010
.byte 0x02 // R 10010100 -> 00000010
.byte 0x02 // R 10010101 -> 00000010
.byte 0x0a // R 10010110 -> 00001010
.byte 0x0a // R 10010111 -> 00001010
.byte 0x02 // R 10011000 -> 00000010
.byte 0x02 // R 10011001 -> 00000010
.byte 0x0a // R 10011010 -> 00001010
.byte 0x0a // R 10011011 -> 00001010
.byte 0x02 // R 10011100 -> 00000010
.byte 0x02 // R 10011101 -> 00000010
.byte 0x0a // R 10011110 -> 00001010
.byte 0x0a // R 10011111 -> 00001010
.byte 0x00 // R 10100000 -> 00000000
.byte 0x00 // R 10100001 -> 00000000
.byte 0x08 // R 10100010 -> 00001000
.byte 0x08 // R 10100011 -> 00001000
.byte 0x00 // R 10100100 -> 00000000
.byte 0x00 // R 10100101 -> 00000000
.byte 0x08 // R 10100110 -> 00001000
.byte 0x08 // R 10100111 -> 00001000
.byte 0x00 // R 10101000 -> 00000000
.byte 0x00 // R 10101001 -> 00000000
.byte 0x08 // R 10101010 -> 00001000
.byte 0x08 // R 10101011 -> 00001000
.byte 0x00 // R 10101100 -> 00000000
.byte 0x00 // R 10101101 -> 00000000
.byte 0x08 // R 10101110 -> 00001000
.byte 0x08 // R 10101111 -> 00001000
.byte 0x02 // R 10110000 -> 00000010
.byte 0x02 // R 10110001 -> 00000010
.byte 0x0a // R 10110010 -> 00001010
.byte 0x0a // R 10110011 -> 00001010
.byte 0x02 // R 10110100 -> 00000010
.byte 0x02 // R 10110101 -> 00000010
.byte 0x0a // R 10110110 -> 00001010
.byte 0x0a // R 10110111 -> 00001010
.byte 0x02 // R 10111000 -> 00000010
.byte 0x02 // R 10111001 -> 00000010
.byte 0x0a // R 10111010 -> 00001010
.byte 0x0a // R 10111011 -> 00001010
.byte 0x02 // R 10111100 -> 00000010
.byte 0x02 // R 10111101 -> 00000010
.byte 0x0a // R 10111110 -> 00001010
.byte 0x0a // R 10111111 -> 00001010
.byte 0x05 // R 11000000 -> 00000101
.byte 0x05 // R 11000001 -> 00000101
.byte 0x0d // R 11000010 -> 00001101
.byte 0x0d // R 11000011 -> 00001101
.byte 0x05 // R 11000100 -> 00000101
.byte 0x05 // R 11000101 -> 00000101
.byte 0x0d // R 11000110 -> 00001101
.byte 0x0d // R 11000111 -> 00001101
.byte 0x05 // R 11001000 -> 00000101
.byte 0x05 // R 11001001 -> 00000101
.byte 0x0d // R 11001010 -> 00001101
.byte 0x0d // R 11001011 -> 00001101
.byte 0x05 // R 11001100 -> 00000101
.byte 0x05 // R 11001101 -> 00000101
.byte 0x0d // R 11001110 -> 00001101
.byte 0x0d // R 11001111 -> 00001101
.byte 0x07 // R 11010000 -> 00000111
.byte 0x07 // R 11010001 -> 00000111
.byte 0x0f // R 11010010 -> 00001111
.byte 0x0f // R 11010011 -> 00001111
.byte 0x07 // R 11010100 -> 00000111
.byte 0x07 // R 11010101 -> 00000111
.byte 0x0f // R 11010110 -> 00001111
.byte 0x0f // R 11010111 -> 00001111
.byte 0x07 // R 11011000 -> 00000111
.byte 0x07 // R 11011001 -> 00000111
.byte 0x0f // R 11011010 -> 00001111
.byte 0x0f // R 11011011 -> 00001111
.byte 0x07 // R 11011100 -> 00000111
.byte 0x07 // R 11011101 -> 00000111
.byte 0x0f // R 11011110 -> 00001111
.byte 0x0f // R 11011111 -> 00001111
.byte 0x05 // R 11100000 -> 00000101
.byte 0x05 // R 11100001 -> 00000101
.byte 0x0d // R 11100010 -> 00001101
.byte 0x0d // R 11100011 -> 00001101
.byte 0x05 // R 11100100 -> 00000101
.byte 0x05 // R 11100101 -> 00000101
.byte 0x0d // R 11100110 -> 00001101
.byte 0x0d // R 11100111 -> 00001101
.byte 0x05 // R 11101000 -> 00000101
.byte 0x05 // R 11101001 -> 00000101
.byte 0x0d // R 11101010 -> 00001101
.byte 0x0d // R 11101011 -> 00001101
.byte 0x05 // R 11101100 -> 00000101
.byte 0x05 // R 11101101 -> 00000101
.byte 0x0d // R 11101110 -> 00001101
.byte 0x0d // R 11101111 -> 00001101
.byte 0x07 // R 11110000 -> 00000111
.byte 0x07 // R 11110001 -> 00000111
.byte 0x0f // R 11110010 -> 00001111
.byte 0x0f // R 11110011 -> 00001111
.byte 0x07 // R 11110100 -> 00000111
.byte 0x07 // R 11110101 -> 00000111
.byte 0x0f // R 11110110 -> 00001111
.byte 0x0f // R 11110111 -> 00001111
.byte 0x07 // R 11111000 -> 00000111
.byte 0x07 // R 11111001 -> 00000111
.byte 0x0f // R 11111010 -> 00001111
.byte 0x0f // R 11111011 -> 00001111
.byte 0x07 // R 11111100 -> 00000111
.byte 0x07 // R 11111101 -> 00000111
.byte 0x0f // R 11111110 -> 00001111
.byte 0x0f // R 11111111 -> 00001111
