#include "ZxSpectrumVideoDriver.h"

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