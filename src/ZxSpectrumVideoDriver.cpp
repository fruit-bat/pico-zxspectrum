#include "ZxSpectrumVideoDriver.h"

#include "ZxDviRenderLoop.h"
#include "ZxSt7789LcdRenderLoop.h"
#include "ZxScanlineVgaRenderLoop.h"

void ZxSpectrumVideoInitNull() {
}

void ZxSpectrumVideoLoopNull(
  ZxSpectrum &zxSpectrum,
  volatile uint &frames,
  bool &showMenu,
  volatile bool &toggleMenu,
  ZxSpectrumMenu& picoRootWin)
{
}

zx_spectrum_video_driver_t _zx_spectrum_video_drivers[ZX_SPECTRUM_VIDEO_DRIVER_COUNT] = {
  {
    ZxSpectrumVideoInitNull,
    ZxSpectrumVideoLoopNull,
    "Null"
  },
  {
    ZxScanlineVgaRenderLoopInit_640x480p_60hz,
    ZxScanlineVgaRenderLoop,
    "VGA 640x480p 60hz"
  },
  {
    ZxScanlineVgaRenderLoopInit_720x576p_50hz,
    ZxScanlineVgaRenderLoop,
    "VGA 720x576p 50hz"
  },
  {
    ZxDviRenderLoopInit_640x480p_60hz,
    ZxDviRenderLoop,
    "DVI 640x480p 60Hz"
  },
  {
    ZxDviRenderLoopInit_720x540p_50hz,
    ZxDviRenderLoop,
    "DVI 720x540p 50hz"
  },
  {
    ZxDviRenderLoopInit_720x576p_50hz,
    ZxDviRenderLoop,
    "DVI 720x576p 50hz"
  },
  {
    ZxSt7789LcdRenderLoopInit,
    ZxSt7789LcdRenderLoop,
    "LCD"
  }
};

// DVI_DEFAULT_SERIAL_CONFIG

void ZxSpectrumVideoInit() {
    /*
#ifdef PICO_ST7789_LCD
    if(zx_fire_raw()) useDvi = !useDvi;
    
    // TODO Check if we are using DVI or LCD
    if (useDvi) {
      // Init the DVI output and set the clock
      ZxDviRenderLoopInit();
    }
    else {
      // TODO without the DVI running we have no clock to sync with!
      // TODO make sure we start an audio option to sync with instead!
      ZxSt7789LcdRenderLoopInit();
    }
#else 
    // Init the DVI output and set the clock
    ZxDviRenderLoopInit();
#endif
*/
}


void ZxSpectrumVideoLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin)
{
    /*
#ifdef PICO_ST7789_LCD
  if (useDvi) {
    ZxDviRenderLoop(
      zxSpectrum,
      _frames,
      showMenu,
      toggleMenu
    );
  }
  else {
    ZxSt7789LcdRenderLoop(
      zxSpectrum,
      _frames,
      showMenu,
      toggleMenu,
      picoRootWin
    );
  }
#else
  ZxDviRenderLoop(
    zxSpectrum,
    _frames,
    showMenu,
    toggleMenu
  );
#endif
*/
}