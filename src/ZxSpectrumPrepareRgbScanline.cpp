#include "ZxSpectrumPrepareRgbScanline.h"
//need port
#include "ZxSpectrum.h"

#define VGA_RGBY_1111(r,g,b,y) ((y##UL<<3)|(r##UL<<2)|(g##UL<<1)|b##UL)
#define VGA_RGB_332(r,g,b) ((r##UL<<5)|(g##UL<<2)|b##UL)
#define VGA_RGB_222(r,g,b) ((r##UL<<4)|(g##UL<<2)|b##UL)

#define X4(a) (a | (a << 8) | (a << 16) | (a << 24))

static uint32_t zx_colour_words[16] = {
#if defined(VGA_ENC_RGBY_1111)
  X4(VGA_RGBY_1111(0,0,0,0)), // Black
  X4(VGA_RGBY_1111(0,0,1,0)), // Blue
  X4(VGA_RGBY_1111(1,0,0,0)), // Red
  X4(VGA_RGBY_1111(1,0,1,0)), // Magenta
  X4(VGA_RGBY_1111(0,1,0,0)), // Green
  X4(VGA_RGBY_1111(0,1,1,0)), // Cyan
  X4(VGA_RGBY_1111(1,1,0,0)), // Yellow
  X4(VGA_RGBY_1111(1,1,1,0)), // White
  X4(VGA_RGBY_1111(0,0,0,0)), // Bright Black
  X4(VGA_RGBY_1111(0,0,1,1)), // Bright Blue
  X4(VGA_RGBY_1111(1,0,0,1)), // Bright Red
  X4(VGA_RGBY_1111(1,0,1,1)), // Bright Magenta
  X4(VGA_RGBY_1111(0,1,0,1)), // Bright Green
  X4(VGA_RGBY_1111(0,1,1,1)), // Bright Cyan
  X4(VGA_RGBY_1111(1,1,0,1)), // Bright Yellow
  X4(VGA_RGBY_1111(1,1,1,1))  // Bright White
#elif defined(VGA_ENC_RGB_222)
  X4(VGA_RGB_222(0,0,0)), // Black
  X4(VGA_RGB_222(0,0,2)), // Blue
  X4(VGA_RGB_222(2,0,0)), // Red
  X4(VGA_RGB_222(2,0,2)), // Magenta
  X4(VGA_RGB_222(0,2,0)), // Green
  X4(VGA_RGB_222(0,2,2)), // Cyan
  X4(VGA_RGB_222(2,2,0)), // Yellow
  X4(VGA_RGB_222(2,2,2)), // White
  X4(VGA_RGB_222(0,0,0)), // Bright Black
  X4(VGA_RGB_222(0,0,3)), // Bright Blue
  X4(VGA_RGB_222(3,0,0)), // Bright Red
  X4(VGA_RGB_222(3,0,3)), // Bright Magenta
  X4(VGA_RGB_222(0,3,0)), // Bright Green
  X4(VGA_RGB_222(0,3,3)), // Bright Cyan
  X4(VGA_RGB_222(3,3,0)), // Bright Yellow
  X4(VGA_RGB_222(3,3,3))  // Bright White
#else // default RGB_332
  X4(VGA_RGB_332(0,0,0)), // Black
  X4(VGA_RGB_332(0,0,2)), // Blue
  X4(VGA_RGB_332(5,0,0)), // Red
  X4(VGA_RGB_332(5,0,2)), // Magenta
  X4(VGA_RGB_332(0,5,0)), // Green
  X4(VGA_RGB_332(0,5,2)), // Cyan
  X4(VGA_RGB_332(5,5,0)), // Yellow
  X4(VGA_RGB_332(5,5,2)), // White
  X4(VGA_RGB_332(0,0,0)), // Bright Black
  X4(VGA_RGB_332(0,0,3)), // Bright Blue
  X4(VGA_RGB_332(7,0,0)), // Bright Red
  X4(VGA_RGB_332(7,0,3)), // Bright Magenta
  X4(VGA_RGB_332(0,7,0)), // Bright Green
  X4(VGA_RGB_332(0,7,3)), // Bright Cyan
  X4(VGA_RGB_332(7,7,0)), // Bright Yellow
  X4(VGA_RGB_332(7,7,3))  // Bright White
#endif
};

static uint32_t zx_bitbit_masks[4] = {
  0x00000000,
  0xffff0000,
  0x0000ffff,
  0xffffffff
};

static uint32_t zx_invert_masks[] = {
  0x00,
  0xff
};

void __not_in_flash_func(zx_prepare_rgb_scanline)(
  uint32_t* buf,
  uint32_t y,
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
  ) {

  const uint32_t bw = zx_colour_words[borderColor];
  const unsigned int rez = (_z80x & 1) ? 128 : 192;
  const int r = zx_colour_words[2];
  const int g = zx_colour_words[4];
  const int b = zx_colour_words[1];

  const uint bord = 24 + ((_z80x & 1) ? 32 : 0);
  const uint v = y - bord;

  if (y < bord || y >= (bord+rez)) {
    // Screen is 640 bytes
    // Each color word is 4 bytes, which represents 2 pixels
    beamColor = 0xff;
    for (int i = 0; i < 160; ++i) buf[i] = bw;
  }
  else {
    // 640 - (256 * 2) = 128
    // Border edge is 64 bytes wide
    for (int i = 0; i < 16; ++i) *buf++ = bw;

    // line in char cell ((v & 0x7) << 8)
    // every 8 lines or char cells ((v & 0x38) << 2)
    // every 64 lines or screen thirds ((v & 0xc0) << 5)
    const uint8_t *s = screenPtr + ((v & 0x7) << 8) + ((v & 0x38) << 2) + ((v & 0xc0) << 5);
    const uint8_t *s2 = s + 32 * 128;
    const uint8_t *s3 = s2 + 32 * 128;
    const uint8_t *a = attrPtr+((v>>3)<<5);
    const int m = (frame >> 5) & 1;
    beamColor = *a;//try beam following only 256*192 screen

    for (int i = 0; i < 32; ++i) {//an rgb based mode extra by colour tables??
      uint8_t c = *a++; // Fetch the attribute for the character
      //=============================
      // get alternates
      //=============================
      if(_z80x & 1) {
        // new video mode ?
        uint8_t ba = *s++; // fetch a byte of pixel data
        uint8_t ra = *s2++; // fetch a byte of pixel data
        uint8_t ga = *s3++; // fetch a byte of pixel data
        //speccy intensity ordering of planes toward high memory

        *buf++ = (zx_bitbit_masks[(ra >> 6) & 3] & r)
          + (zx_bitbit_masks[(ga >> 6) & 3] & g)
          + (zx_bitbit_masks[(ba >> 6) & 3] & b);
        *buf++ = (zx_bitbit_masks[(ra >> 4) & 3] & r)
          + (zx_bitbit_masks[(ga >> 4) & 3] & g)
          + (zx_bitbit_masks[(ba >> 4) & 3] & b);
        *buf++ = (zx_bitbit_masks[(ra >> 2) & 3] & r)
          + (zx_bitbit_masks[(ga >> 2) & 3] & g)
          + (zx_bitbit_masks[(ba >> 2) & 3] & b);
        *buf++ = (zx_bitbit_masks[(ra >> 0) & 3] & r)
          + (zx_bitbit_masks[(ga >> 0) & 3] & g)
          + (zx_bitbit_masks[(ba >> 0) & 3] & b);
      } else {
        uint8_t p = *s++ ^ zx_invert_masks[(c >> 7) & m]; // fetch a byte of pixel data
        uint8_t bci = (c >> 3) & 0xf; // The background colour index
        uint8_t fci = (c & 7) | (bci & 0x8); // The foreground colour index
        // back on track
        uint32_t bcw = zx_colour_words[bci]; // The background colour word
        uint32_t fcw = zx_colour_words[fci]; // The foreground colour word
        uint32_t fgm;
        uint32_t bgm;
        fgm = zx_bitbit_masks[(p >> 6) & 3];
        bgm = ~fgm;
        *buf++ = (fgm & fcw) | (bgm & bcw);
        fgm = zx_bitbit_masks[(p >> 4) & 3];
        bgm = ~fgm;
        *buf++ = (fgm & fcw) | (bgm & bcw);
        fgm = zx_bitbit_masks[(p >> 2) & 3];
        bgm = ~fgm;
        *buf++ = (fgm & fcw) | (bgm & bcw);
        fgm = zx_bitbit_masks[p & 3];
        bgm = ~fgm;
        *buf++ = (fgm & fcw) | (bgm & bcw);
      }
    }

    for (int i = 0; i < 16; ++i) *buf++ = bw;
  }
}
