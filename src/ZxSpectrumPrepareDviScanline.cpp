#include "ZxSpectrumPrepareDviScanline.h"
//need port
#include "ZxSpectrum.h"

extern "C" {
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"
#include "tmds_encode_zxspectrum.h"
}

//=======================================================
// N.B. INDEXING MAYBE OFF ....................
//=======================================================
// this fixes bitmap memory ordering intensity based luma
const uint8_t primaries[3] = {
    64+1, //BLUE
    64+2, //RED
    64+4, //GREEN
};

// this is for the output draw plane ordering ...
const uint8_t indexing[3] = {
  0, 2, 1,//ORDER MAY NEED CHANGING
  // Assuming https://en.wikipedia.org/wiki/Digital_Visual_Interface
  // is correct starting with pin 17 ... BGR
};

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
  const unsigned int rez = (_z80x & 1) ? 128 : 192;
  const uint bord = 24 + ((_z80x & 1) ? 32 : 0);
  const uint v = y - bord;
  const uint8_t *s = screenPtr + ((v & 0x7) << 8) + ((v & 0x38) << 2) + ((v & 0xc0) << 5);
  const uint8_t *a = attrPtr+((v>>3)<<5);
  const int m = (frame >> 5) & 1 ? 0xff : 0x7f;

  if (y < bord || y >= (bord+rez)) {
    for (int plane = 0; plane < 3; ++plane) {
      p = tmds_encode_border(
        40,          // r0 is width in characters
        plane,       // r1 is colour channel
        p,           // r2 is output TMDS buffer
        borderColor  // r3 is the colour attribute
      );
    }
  }
  else {
    for (int plane = 0; plane < 3; ++plane) {
      p = tmds_encode_border(
        4,           // r0 is width in characters
        plane,       // r1 is colour channel
        p,           // r2 is output TMDS buffer
        borderColor  // r3 is the colour attribute
      );
      if(_z80x & 1) {
        // new video mode ?
        p = tmds_encode_screen(//an rgb based mode extra by colour tables??
          (const uint8_t*)(s + (indexing[plane] * 32 * 128)),
          &primaries[indexing[plane]],//attribute primary
          p,
          m,
          plane
        );
      } else {
        p = tmds_encode_screen(//an rgb based mode extra by colour tables??
          (const uint8_t*)s,
          a,
          p,
          m,
          plane
        );
      }
      p = tmds_encode_border(
        4,           // r0 is width in characters
        plane,       // r1 is colour channel
        p,           // r2 is output TMDS buffer
        borderColor  // r3 is the colour attribute
      );
    }
  }
  queue_add_blocking(&dvi->q_tmds_valid, &tmdsbuf);
}
