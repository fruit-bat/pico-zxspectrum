#include "ZxSpectrumPrepareRgb16Scanline.h"

#define VGA_RGB_555(r,g,b) ((r##UL<<0)|(g##UL<<5)|(b##UL << 10))

#define X2(a) (a | (a << 16))

static uint32_t zx_colour_words[16] = {
  X2(VGA_RGB_555(0, 0, 0 )), // Black
  X2(VGA_RGB_555(0, 0, 15)), // Blue
  X2(VGA_RGB_555(15,0, 0 )), // Red
  X2(VGA_RGB_555(15,0, 15)), // Magenta
  X2(VGA_RGB_555(0, 15,0 )), // Green
  X2(VGA_RGB_555(0, 15,15)), // Cyan
  X2(VGA_RGB_555(15,15,0 )), // Yellow
  X2(VGA_RGB_555(15,15,15)), // White
  X2(VGA_RGB_555(0, 0, 0 )), // Bright Black
  X2(VGA_RGB_555(0, 0, 31)), // Bright Blue
  X2(VGA_RGB_555(31,0, 0 )), // Bright Red
  X2(VGA_RGB_555(31,0, 31)), // Bright Magenta
  X2(VGA_RGB_555(0, 31,0 )), // Bright Green
  X2(VGA_RGB_555(0, 31,31)), // Bright Cyan
  X2(VGA_RGB_555(31,31,0 )), // Bright Yellow
  X2(VGA_RGB_555(31,31,31))  // Bright White
};

static uint32_t zx_invert_masks[] = {
  0x00,
  0xff
};

void __not_in_flash_func(zx_prepare_rgb16_scanline)(
  uint32_t* buf, 
  uint32_t y, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
  ) {

  const uint32_t bw = zx_colour_words[borderColor];
  
  if (y < 24 || y >= (24+192)) {
    // Screen is 640 bytes
    // Each color word is 4 bytes, which represents 1 pixels
    for (int i = 0; i < 320; ++i) buf[i] = bw;
  }
  else {
    // 640 - (256 * 2) = 128
    // Border edge is 128 bytes wide
    for (int i = 0; i < 32; ++i) *buf++ = bw;
    
    const uint v = y - 24;
    const uint8_t *s = screenPtr + ((v & 0x7) << 8) + ((v & 0x38) << 2) + ((v & 0xc0) << 5);
    const uint8_t *a = attrPtr+((v>>3)<<5);
    const int m = (frame >> 5) & 1;   
     
    for (int i = 0; i < 32; ++i) {
      uint8_t c = *a++; // Fetch the attribute for the character
      uint8_t p = *s++ ^ zx_invert_masks[(c >> 7) & m]; // fetch a byte of pixel data
      uint8_t bci = (c >> 3) & 0xf; // The background colour index
      uint8_t fci = (c & 7) | (bci & 0x8); // The foreground colour index
      
      uint32_t cw[2];
      cw[0] = zx_colour_words[bci]; // The background colour word
      cw[1] = zx_colour_words[fci]; // The foreground colour word
      
      *buf++ = cw[(p >> 7) & 1];
      *buf++ = cw[(p >> 6) & 1];
      *buf++ = cw[(p >> 5) & 1];
      *buf++ = cw[(p >> 4) & 1];
      *buf++ = cw[(p >> 3) & 1];
      *buf++ = cw[(p >> 2) & 1];
      *buf++ = cw[(p >> 1) & 1];
      *buf++ = cw[(p >> 0) & 1];
    }
    
    for (int i = 0; i < 32; ++i) *buf++ = bw;
  }
}


