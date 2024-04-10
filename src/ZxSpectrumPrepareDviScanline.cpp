#include "ZxSpectrumPrepareDviScanline.h"

extern "C" {
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"
#include "tmds_encode_zxspectrum.h"
}
#include "ZxSpectrumDisplay.h"

// Screen handler
//
// 256×192 Pixels
// 32x24 Charaters
//
// 240-192 = 48 => 24 border rows top and bottom
// 320-256 = 64 => 64 border pixels left and right
//
void __not_in_flash_func(zx_prepare_hdmi_scanline)(
  struct dvi_inst *dvi, 
  uint32_t y, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
) {  
  uint32_t *tmdsbuf;
  queue_remove_blocking(&dvi->q_tmds_free, &tmdsbuf);
  uint32_t *p = tmdsbuf;
  const uint v = y - 24;
  const uint8_t *s = screenPtr + ((v & 0x7) << 8) + ((v & 0x38) << 2) + ((v & 0xc0) << 5);
  const uint8_t *a = attrPtr+((v>>3)<<5);
  const int m = (frame >> 5) & 1 ? 0xff : 0x7f;
  
  if (y < 24 || y >= (24+192)) {
    for (int plane = 0; plane < 3; ++plane) {
#if DISPLAY_BORDER_PIXELS_LEFT_BLANK      
      p = tmds_encode_border(
        DISPLAY_BORDER_PIXELS_LEFT_BLANK, // r0 is width in pixels
        plane,                       // r1 is colour channel
        p,                           // r2 is output TMDS buffer
        0                            // r3 is the colour attribute
      );
#endif
      p = tmds_encode_border(
        DISPLAY_BORDER_PIXELS
          - DISPLAY_BORDER_PIXELS_LEFT_BLANK
          - DISPLAY_BORDER_PIXELS_RIGHT_BLANK, // r0 is width in pixels
        plane,                       // r1 is colour channel
        p,                           // r2 is output TMDS buffer
        borderColor                  // r3 is the colour attribute
      );
#if DISPLAY_BORDER_PIXELS_RIGHT_BLANK
      p = tmds_encode_border(
        DISPLAY_BORDER_PIXELS_RIGHT_BLANK, // r0 is width in pixels
        plane,                       // r1 is colour channel
        p,                           // r2 is output TMDS buffer
        0                            // r3 is the colour attribute
      ); 
#endif
    }
  }
  else {
    for (int plane = 0; plane < 3; ++plane) {
#if DISPLAY_BORDER_PIXELS_LEFT_BLANK      
      p = tmds_encode_border(
        DISPLAY_BORDER_PIXELS_LEFT_BLANK, // r0 is width in pixels
        plane,                       // r1 is colour channel
        p,                           // r2 is output TMDS buffer
        0                            // r3 is the colour attribute
      );      
#endif
      p = tmds_encode_border(
        DISPLAY_BORDER_PIXELS_LEFT_COLORED,  // r0 is width in pixels
        plane,                       // r1 is colour channel
        p,                           // r2 is output TMDS buffer
        borderColor                  // r3 is the colour attribute
      );
      p = tmds_encode_screen(
        (const uint8_t*)s,
        a,
        p,
        m,
        plane
      );  
      p = tmds_encode_border(
        DISPLAY_BORDER_PIXELS_RIGHT_COLORED, // r0 is width in pixels
        plane,                       // r1 is colour channel
        p,                           // r2 is output TMDS buffer
        borderColor                  // r3 is the colour attribute
      );
#if DISPLAY_BORDER_PIXELS_RIGHT_BLANK
      p = tmds_encode_border(
        DISPLAY_BORDER_PIXELS_RIGHT_BLANK, // r0 is width in pixels
        plane,                       // r1 is colour channel
        p,                           // r2 is output TMDS buffer
        0                            // r3 is the colour attribute
      );
#endif
    }
  }
  queue_add_blocking(&dvi->q_tmds_valid, &tmdsbuf);
}
