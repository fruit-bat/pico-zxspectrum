#include "ZxSpectrumVideoDriver.h"

#if defined(DVI_DEFAULT_SERIAL_CONFIG)
#include "ZxDviRenderLoop.h"
#else
#define ZxDviRenderLoopInit NULL 
#define ZxDviRenderLoop NULL
#endif

#if defined(PICO_ST7789_LCD)
#include "ZxSt7789LcdRenderLoop.h"
#else
#define ZxSt7789LcdRenderLoopInit NULL
#define ZxSt7789LcdRenderLoop NULL
#endif

#if defined(CVBS_12MHZ) || defined(CVBS_13_5MHZ) || defined(VGA_MODE)
#include "ZxScanlineVgaRenderLoop.h"
#else
#define ZxScanlineVgaRenderLoopInit NULL
#define ZxScanlineVgaRenderLoop NULL
#endif

zx_spectrum_video_driver_t _zx_spectrum_video_drivers[ZX_SPECTRUM_VIDEO_DRIVER_COUNT] = {  
  {
    ZxScanlineVgaRenderLoopInit,
    ZxScanlineVgaRenderLoop,
    "VGA"
  },
  {
    ZxDviRenderLoopInit,
    ZxDviRenderLoop,
    "DVI"
  },
  {
    ZxSt7789LcdRenderLoopInit,
    ZxSt7789LcdRenderLoop,
    "LCD"
  }
};

static zx_spectrum_video_driver_enum_t _video_driver_index = DEFAULT_VIDEO_DRIVER;

void ZxSpectrumVideoInit() {
  zx_spectrum_video_init_t init = _zx_spectrum_video_drivers[_video_driver_index].init;
  if (init) init();
}

void ZxSpectrumVideoLoop(
  ZxSpectrum &zxSpectrum,
  volatile uint &frames,
  bool &showMenu,
  volatile bool &toggleMenu,
  ZxSpectrumMenu& picoRootWin)
{
  zx_spectrum_video_loop_t loop = _zx_spectrum_video_drivers[_video_driver_index].loop;  
  if (loop) loop(
      zxSpectrum,
      frames,
      showMenu,
      toggleMenu,
      picoRootWin
  );
}
