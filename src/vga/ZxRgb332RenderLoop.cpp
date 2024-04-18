#include "ZxRgb332RenderLoop.h"
#include "ZxSpectrumDisplay.h"
#include "vga.h"
#include "ZxSpectrumPrepareRgbScanline.h"
#include "PicoCharRendererVga.h"

static sVmode vmode;

void ZxRgb332RenderLoopInit() {
  sVgaCfg cfg;
  cfg.width = DISPLAY_WIDTH_PIXELS;       // width in pixels
  cfg.height = DISPLAY_HEIGHT_PIXELS / 2; // height in lines
  cfg.wfull = 0;                          // width of full screen, corresponding to 'hfull' time (0=use 'width' parameter)
  cfg.video = &VGA_MODE;                  // used video timings
  cfg.freq = 180000;                      // required minimal system frequency in kHz (real frequency can be higher)
  cfg.fmax = 280000;                      // maximal system frequency in kHz (limit resolution if needed)
  cfg.dbly = true;                        // double in Y direction
  cfg.lockfreq = false;                   // lock required frequency, do not change it
  VgaCfg(&cfg, &vmode);                   // calculate videomode setup

  // initialize system clock
  set_sys_clock_pll(vmode.vco * 1000, vmode.pd1, vmode.pd2);
}

void __not_in_flash_func(ZxRgb332RenderLoop)(
  ZxSpectrum &zxSpectrum, 
  uint &frames,
  bool &showMenu,
  bool &toggleMenu
) {
 
  VgaInit(&vmode);

  unsigned char* screenPtr = zxSpectrum.screenPtr();
  unsigned char* attrPtr = screenPtr + (32 * 24 * 8);

  while (true) {
    VgaLineBuf *linebuf = get_vga_line();
    uint32_t* buf = (uint32_t*)&(linebuf->line);
    uint32_t y = linebuf->row;

    const uint32_t fpf = zxSpectrum.flipsPerFrame();
    const bool ringoMode = fpf > 46 && fpf < 52;
    if (ringoMode) {
      screenPtr = zxSpectrum.memPtr(y & 4 ? 7 : 5);
      attrPtr = screenPtr + (32 * 24 * 8);
    }
    const uint32_t blankTopLines = (DISPLAY_BLANK_LINES/2);

    if (y == blankTopLines) {
      
      if (!ringoMode) {
        screenPtr = zxSpectrum.screenPtr();
        attrPtr = screenPtr + (32 * 24 * 8);
      }
      
      if (toggleMenu) {
        showMenu = !showMenu;
        toggleMenu = false;
        ZxRgb332RenderLoopCallbackMenu(showMenu);
      }
      
      frames = linebuf->frame;
    }

    if (y < blankTopLines || y >= (blankTopLines + ZX_SPECTRUM_SCREEN_HEIGHT)) {
      zx_prepare_rgb_blankline(
        buf
      );
    }
    else if (showMenu) {
      pcw_prepare_vga332_scanline_80(
        buf,
        y - blankTopLines,
        linebuf->frame);
    }
    else { 
      zx_prepare_rgb_scanline(
        buf, 
        y - blankTopLines, 
        linebuf->frame,
        screenPtr,
        attrPtr,
        zxSpectrum.borderColour(y - blankTopLines)
      );

    }  

    ZxRgb332RenderLoopCallbackLine(y);
  }

  __builtin_unreachable();
}
