#include "pzx_prepare_rgb444_scanline.h"
#include "st7789_lcd.pio.h"

#ifdef INVERSE_LCD
#define VGA_COL(c) (0xf-c##UL)
#else
#define VGA_COL(c) (c##UL)
#endif
#define VGA_RGB_444_POS(r,g,b) ((r<<8)|(g<<4)|b)
#define VGA_RGB_444(r,g,b) VGA_RGB_444_POS(VGA_COL(r),VGA_COL(g), VGA_COL(b))
#define X2(a) ((a << 20) | (a << 8))

static uint32_t zx_colour_words[16] = {
  X2(VGA_RGB_444(0x0,0x0,0x0)), // Black
  X2(VGA_RGB_444(0x0,0x0,0xc)), // Blue
  X2(VGA_RGB_444(0xc,0x0,0x0)), // Red
  X2(VGA_RGB_444(0xc,0x0,0xc)), // Magenta
  X2(VGA_RGB_444(0x0,0xc,0x0)), // Green
  X2(VGA_RGB_444(0x0,0xc,0xc)), // Cyan
  X2(VGA_RGB_444(0xc,0xc,0x0)), // Yellow
  X2(VGA_RGB_444(0xc,0xc,0xc)), // White
  X2(VGA_RGB_444(0x0,0x0,0x0)), // Bright Black
  X2(VGA_RGB_444(0x0,0x0,0xf)), // Bright Blue
  X2(VGA_RGB_444(0xf,0x0,0x0)), // Bright Red
  X2(VGA_RGB_444(0xf,0x0,0xf)), // Bright Magenta
  X2(VGA_RGB_444(0x0,0xf,0x0)), // Bright Green
  X2(VGA_RGB_444(0x0,0xf,0xf)), // Bright Cyan
  X2(VGA_RGB_444(0xf,0xf,0x0)), // Bright Yellow
  X2(VGA_RGB_444(0xf,0xf,0xf))  // Bright White
};

static uint32_t zx_bitbit_masks[4] = {
  0x00000000,
  0x000fff00,
  0xfff00000,
  0xffffff00
};

static uint32_t zx_invert_masks[] = {
  0x00,
  0xff
};

void __not_in_flash_func(pzx_send_rgb444_scanline)(
  PIO pio,
  uint sm, 
  uint32_t y, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
  ) {

  const uint32_t bw = zx_colour_words[borderColor];
  
  if (y < 24 || y >= (24+192)) {
    // Screen is 640 bytes
    // Each color word is 4 bytes, which represents 2 pixels
    for (int i = 0; i < 160; ++i) st7789_lcd_put_rgb444(pio, sm, bw);
  }
  else {
    // 640 - (256 * 2) = 128
    // Border edge is 64 bytes wide
    for (int i = 0; i < 16; ++i) st7789_lcd_put_rgb444(pio, sm, bw);
    
    const uint v = y - 24;
    const uint8_t *s = screenPtr + ((v & 0x7) << 8) + ((v & 0x38) << 2) + ((v & 0xc0) << 5);
    const uint8_t *a = attrPtr+((v>>3)<<5);
    const int m = (frame >> 5) & 1;   
     
    for (int i = 0; i < 32; ++i) {
      uint8_t c = *a++; // Fetch the attribute for the character
      uint8_t p = *s++ ^ zx_invert_masks[(c >> 7) & m]; // fetch a byte of pixel data
      uint8_t bci = (c >> 3) & 0xf; // The background colour index
      uint8_t fci = (c & 7) | (bci & 0x8); // The foreground colour index
      uint32_t bcw = zx_colour_words[bci]; // The background colour word
      uint32_t fcw = zx_colour_words[fci]; // The foreground colour word
      uint32_t fgm;
      uint32_t bgm;
      fgm = zx_bitbit_masks[(p >> 6) & 3];
      bgm = ~fgm;
      st7789_lcd_put_rgb444(pio, sm, (fgm & fcw) | (bgm & bcw));
      fgm = zx_bitbit_masks[(p >> 4) & 3];
      bgm = ~fgm;
      st7789_lcd_put_rgb444(pio, sm, (fgm & fcw) | (bgm & bcw));     
      fgm = zx_bitbit_masks[(p >> 2) & 3];
      bgm = ~fgm;
      st7789_lcd_put_rgb444(pio, sm, (fgm & fcw) | (bgm & bcw));     
      fgm = zx_bitbit_masks[p & 3];
      bgm = ~fgm;
      st7789_lcd_put_rgb444(pio, sm, (fgm & fcw) | (bgm & bcw));           
    }
    
    for (int i = 0; i < 16; ++i) st7789_lcd_put_rgb444(pio, sm, bw);
  }
}

void __not_in_flash_func(pzx_send_rgb444_frame)(
  PIO pio,
  uint sm, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
) {
  for (uint32_t y = 0; y < 240; ++y) pzx_send_rgb444_scanline(pio, sm, y, frame, screenPtr, attrPtr, borderColor);
}
