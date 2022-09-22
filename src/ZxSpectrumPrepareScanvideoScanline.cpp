#include "ZxSpectrumPrepareScanvideoScanline.h"
#include "pico/scanvideo/composable_scanline.h"

#define VGA_RGB_555(r,g,b) ((r##UL<<0)|(g##UL<<6)|(b##UL << 11))

#define X2(a) (a | (a << 16))
#define XH(a) (a << 16)

static uint32_t zx_colour_words[16] = {
  (VGA_RGB_555(0, 0, 0 )), // Black
  (VGA_RGB_555(0, 0, 15)), // Blue
  (VGA_RGB_555(15,0, 0 )), // Red
  (VGA_RGB_555(15,0, 15)), // Magenta
  (VGA_RGB_555(0, 15,0 )), // Green
  (VGA_RGB_555(0, 15,15)), // Cyan
  (VGA_RGB_555(15,15,0 )), // Yellow
  (VGA_RGB_555(15,15,15)), // White
  (VGA_RGB_555(0, 0, 0 )), // Bright Black
  (VGA_RGB_555(0, 0, 31)), // Bright Blue
  (VGA_RGB_555(31,0, 0 )), // Bright Red
  (VGA_RGB_555(31,0, 31)), // Bright Magenta
  (VGA_RGB_555(0, 31,0 )), // Bright Green
  (VGA_RGB_555(0, 31,31)), // Bright Cyan
  (VGA_RGB_555(31,31,0 )), // Bright Yellow
  (VGA_RGB_555(31,31,31))  // Bright White
};

static uint32_t zxd_colour_words[16] = {
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

static uint32_t zxh_colour_words[16] = {
  XH(VGA_RGB_555(0, 0, 0 )), // Black
  XH(VGA_RGB_555(0, 0, 15)), // Blue
  XH(VGA_RGB_555(15,0, 0 )), // Red
  XH(VGA_RGB_555(15,0, 15)), // Magenta
  XH(VGA_RGB_555(0, 15,0 )), // Green
  XH(VGA_RGB_555(0, 15,15)), // Cyan
  XH(VGA_RGB_555(15,15,0 )), // Yellow
  XH(VGA_RGB_555(15,15,15)), // White
  XH(VGA_RGB_555(0, 0, 0 )), // Bright Black
  XH(VGA_RGB_555(0, 0, 31)), // Bright Blue
  XH(VGA_RGB_555(31,0, 0 )), // Bright Red
  XH(VGA_RGB_555(31,0, 31)), // Bright Magenta
  XH(VGA_RGB_555(0, 31,0 )), // Bright Green
  XH(VGA_RGB_555(0, 31,31)), // Bright Cyan
  XH(VGA_RGB_555(31,31,0 )), // Bright Yellow
  XH(VGA_RGB_555(31,31,31))  // Bright White
};

static uint32_t zx_invert_masks[] = {
  0x00,
  0xff
};

inline uint32_t* single_color_run(uint32_t *buf, uint32_t width, uint32_t ci) {

  // | jmp color_run | color | count-3 |  buf[0] =
  *buf++ = COMPOSABLE_COLOR_RUN | zxh_colour_words[ci];
  *buf++ = (width - 3 - 2) | (COMPOSABLE_RAW_2P << 16);
  *buf++ = zxd_colour_words[ci];
  return buf;
}

inline uint32_t* end_run(uint32_t *buf) {
  // end with a black pixel
  *buf++ = COMPOSABLE_RAW_1P;
  *buf++ = COMPOSABLE_EOL_SKIP_ALIGN;
  return buf;
}
  
    
void __not_in_flash_func(zx_prepare_scanvideo_scanline)(
  struct scanvideo_scanline_buffer *scanline_buffer,
  uint32_t y, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
) {
  uint32_t* buf = scanline_buffer->data;

  if (y < 24 || y >= (24+192)) {
    // Screen is 640 bytes
    // Each color word is 4 bytes, which represents 1 pixels
    buf = single_color_run(buf, 640, borderColor);
  }
  else {
    // 640 - (256 * 2) = 128
    buf = single_color_run(buf, 64, borderColor);
    
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
      cw[0] = zxd_colour_words[bci]; // The background colour word
      cw[1] = zxd_colour_words[fci]; // The foreground colour word
      
      uint32_t cwb = cw[(p >> 7) & 1];
      *buf++ = COMPOSABLE_RAW_RUN | (cwb << 16);
      *buf++ = (16 - 3) | (cwb << 16);
      *buf++ = cw[(p >> 6) & 1];
      *buf++ = cw[(p >> 5) & 1];
      *buf++ = cw[(p >> 4) & 1];
      *buf++ = cw[(p >> 3) & 1];
      *buf++ = cw[(p >> 2) & 1];
      *buf++ = cw[(p >> 1) & 1];
      *buf++ = cw[(p >> 0) & 1];
    }
    
    buf = single_color_run(buf, 64, borderColor);
  }
  buf = end_run(buf);
  
  scanline_buffer->data_used = buf - scanline_buffer->data;
  scanline_buffer->status = SCANLINE_OK;  
}


