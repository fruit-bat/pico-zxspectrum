#include "ZxSpectrumPrepareScanvideoScanline.h"
#include "pico/scanvideo/composable_scanline.h"
#include "ZxSpectrumDisplay.h"

#define VGA_RGB_555(r,g,b) ((r##UL<<0)|(g##UL<<6)|(b##UL << 11))
#define VGA_BGYR_1111(r,g,b,y) ((y##UL<<2)|(r##UL<<3)|(g##UL<<1)|b##UL)
#define VGA_RGBY_1111(r,g,b,y) ((y##UL<<3)|(r##UL<<2)|(g##UL<<1)|b##UL)
#define VGA_RGB_332(r,g,b) ((r##UL<<5)|(g##UL<<2)|b##UL)
#define VGA_RGB_222(r,g,b) ((r##UL<<4)|(g##UL<<2)|b##UL)

#define X2(a) ((a) | (a << 16))

static uint32_t zxd_colour_words[16] = {
#if defined(VGA_ENC_BGYR_1111)
  X2(VGA_BGYR_1111(0,0,0,0)), // Black
  X2(VGA_BGYR_1111(0,0,1,0)), // Blue
  X2(VGA_BGYR_1111(1,0,0,0)), // Red
  X2(VGA_BGYR_1111(1,0,1,0)), // Magenta
  X2(VGA_BGYR_1111(0,1,0,0)), // Green
  X2(VGA_BGYR_1111(0,1,1,0)), // Cyan
  X2(VGA_BGYR_1111(1,1,0,0)), // Yellow
  X2(VGA_BGYR_1111(1,1,1,0)), // White
  X2(VGA_BGYR_1111(0,0,0,0)), // Bright Black
  X2(VGA_BGYR_1111(0,0,1,1)), // Bright Blue
  X2(VGA_BGYR_1111(1,0,0,1)), // Bright Red
  X2(VGA_BGYR_1111(1,0,1,1)), // Bright Magenta
  X2(VGA_BGYR_1111(0,1,0,1)), // Bright Green
  X2(VGA_BGYR_1111(0,1,1,1)), // Bright Cyan
  X2(VGA_BGYR_1111(1,1,0,1)), // Bright Yellow
  X2(VGA_BGYR_1111(1,1,1,1))  // Bright White
#elif defined(VGA_ENC_RGBY_1111)
  X2(VGA_RGBY_1111(0,0,0,0)), // Black
  X2(VGA_RGBY_1111(0,0,1,0)), // Blue
  X2(VGA_RGBY_1111(1,0,0,0)), // Red
  X2(VGA_RGBY_1111(1,0,1,0)), // Magenta
  X2(VGA_RGBY_1111(0,1,0,0)), // Green
  X2(VGA_RGBY_1111(0,1,1,0)), // Cyan
  X2(VGA_RGBY_1111(1,1,0,0)), // Yellow
  X2(VGA_RGBY_1111(1,1,1,0)), // White
  X2(VGA_RGBY_1111(0,0,0,0)), // Bright Black
  X2(VGA_RGBY_1111(0,0,1,1)), // Bright Blue
  X2(VGA_RGBY_1111(1,0,0,1)), // Bright Red
  X2(VGA_RGBY_1111(1,0,1,1)), // Bright Magenta
  X2(VGA_RGBY_1111(0,1,0,1)), // Bright Green
  X2(VGA_RGBY_1111(0,1,1,1)), // Bright Cyan
  X2(VGA_RGBY_1111(1,1,0,1)), // Bright Yellow
  X2(VGA_RGBY_1111(1,1,1,1))  // Bright White
#elif defined(VGA_ENC_RGB_222)
  X2(VGA_RGB_222(0,0,0)), // Black
  X2(VGA_RGB_222(0,0,2)), // Blue
  X2(VGA_RGB_222(2,0,0)), // Red
  X2(VGA_RGB_222(2,0,2)), // Magenta
  X2(VGA_RGB_222(0,2,0)), // Green
  X2(VGA_RGB_222(0,2,2)), // Cyan
  X2(VGA_RGB_222(2,2,0)), // Yellow
  X2(VGA_RGB_222(2,2,2)), // White
  X2(VGA_RGB_222(0,0,0)), // Bright Black
  X2(VGA_RGB_222(0,0,3)), // Bright Blue
  X2(VGA_RGB_222(3,0,0)), // Bright Red
  X2(VGA_RGB_222(3,0,3)), // Bright Magenta
  X2(VGA_RGB_222(0,3,0)), // Bright Green
  X2(VGA_RGB_222(0,3,3)), // Bright Cyan
  X2(VGA_RGB_222(3,3,0)), // Bright Yellow
  X2(VGA_RGB_222(3,3,3))  // Bright White
#elif defined(VGA_ENC_RGB_332)
  X2(VGA_RGB_332(0,0,0)), // Black
  X2(VGA_RGB_332(0,0,2)), // Blue
  X2(VGA_RGB_332(5,0,0)), // Red
  X2(VGA_RGB_332(5,0,2)), // Magenta
  X2(VGA_RGB_332(0,5,0)), // Green
  X2(VGA_RGB_332(0,5,2)), // Cyan
  X2(VGA_RGB_332(5,5,0)), // Yellow
  X2(VGA_RGB_332(5,5,2)), // White
  X2(VGA_RGB_332(0,0,0)), // Bright Black
  X2(VGA_RGB_332(0,0,3)), // Bright Blue
  X2(VGA_RGB_332(7,0,0)), // Bright Red
  X2(VGA_RGB_332(7,0,3)), // Bright Magenta
  X2(VGA_RGB_332(0,7,0)), // Bright Green
  X2(VGA_RGB_332(0,7,3)), // Bright Cyan
  X2(VGA_RGB_332(7,7,0)), // Bright Yellow
  X2(VGA_RGB_332(7,7,3))  // Bright White  
#else    
  X2(VGA_RGB_555(0, 0, 0 )), // Black
  X2(VGA_RGB_555(0, 0, 20)), // Blue
  X2(VGA_RGB_555(20,0, 0 )), // Red
  X2(VGA_RGB_555(20,0, 20)), // Magenta
  X2(VGA_RGB_555(0, 20,0 )), // Green
  X2(VGA_RGB_555(0, 20,20)), // Cyan
  X2(VGA_RGB_555(20,20,0 )), // Yellow
  X2(VGA_RGB_555(20,20,20)), // White
  X2(VGA_RGB_555(0, 0, 0 )), // Bright Black
  X2(VGA_RGB_555(0, 0, 31)), // Bright Blue
  X2(VGA_RGB_555(31,0, 0 )), // Bright Red
  X2(VGA_RGB_555(31,0, 31)), // Bright Magenta
  X2(VGA_RGB_555(0, 31,0 )), // Bright Green
  X2(VGA_RGB_555(0, 31,31)), // Bright Cyan
  X2(VGA_RGB_555(31,31,0 )), // Bright Yellow
  X2(VGA_RGB_555(31,31,31))  // Bright White
#endif
};

static uint32_t zx_invert_masks[] = {
  0x00,
  0xff
};

inline uint32_t* single_color_run(uint32_t *buf, uint32_t width, uint32_t ci) {
  // | jmp color_run | color | count-3 |  buf[0] =
  *buf++ = COMPOSABLE_COLOR_RUN | (zxd_colour_words[ci] << 16);
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

  if (y < (24) || y >= (24 + 192)) {
#if DISPLAY_BORDER_PIXELS_LEFT_BLANK      
    buf = single_color_run(buf, DISPLAY_BORDER_PIXELS_LEFT_BLANK << 1, 0);     
#endif
    buf = single_color_run(
      buf, 
      (DISPLAY_BORDER_PIXELS
          - DISPLAY_BORDER_PIXELS_LEFT_BLANK
          - DISPLAY_BORDER_PIXELS_RIGHT_BLANK ) << 1
      , borderColor);

#if DISPLAY_BORDER_PIXELS_RIGHT_BLANK      
    buf = single_color_run(buf, DISPLAY_BORDER_PIXELS_RIGHT_BLANK << 1, 0);     
#endif   
  } 
  else {
#if DISPLAY_BORDER_PIXELS_LEFT_BLANK      
    buf = single_color_run(buf, DISPLAY_BORDER_PIXELS_LEFT_BLANK << 1, 0);     
#endif
    buf = single_color_run(buf, DISPLAY_BORDER_PIXELS_LEFT_COLORED << 1, borderColor);
    
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
    
    buf = single_color_run(buf, DISPLAY_BORDER_PIXELS_RIGHT_COLORED << 1, borderColor);
#if DISPLAY_BORDER_PIXELS_RIGHT_BLANK      
    buf = single_color_run(buf, DISPLAY_BORDER_PIXELS_RIGHT_BLANK << 1, 0);     
#endif    
  }
  buf = end_run(buf);
  
  scanline_buffer->data_used = buf - scanline_buffer->data;
  scanline_buffer->status = SCANLINE_OK;  
}

void __not_in_flash_func(zx_prepare_scanvideo_blankline)(
  struct scanvideo_scanline_buffer *scanline_buffer
) {
  uint32_t* buf = scanline_buffer->data;
  buf = single_color_run(buf, DISPLAY_WIDTH_PIXELS, 0);
  buf = end_run(buf);  
  scanline_buffer->data_used = buf - scanline_buffer->data;
  scanline_buffer->status = SCANLINE_OK;  
}
