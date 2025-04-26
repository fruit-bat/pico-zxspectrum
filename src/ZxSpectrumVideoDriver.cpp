#include "ZxSpectrumVideoDriver.h"

#if defined(PICO_ST7789_LCD)
#include "ZxSt7789LcdRenderLoop.h"
#ifndef DEFAULT_VIDEO_DRIVER
#define DEFAULT_VIDEO_DRIVER zx_spectrum_video_driver_lcd_index
#endif
#else
#define ZxSt7789LcdRenderLoopInit NULL
#define ZxSt7789LcdRenderLoop NULL
#endif

#if defined(DVI_DEFAULT_SERIAL_CONFIG)
#include "ZxDviRenderLoop.h"
#ifndef DEFAULT_VIDEO_DRIVER
#define DEFAULT_VIDEO_DRIVER zx_spectrum_video_driver_dvi_index
#endif
#else
#define ZxDviRenderLoopInit NULL 
#define ZxDviRenderLoop NULL
#endif

#if defined(CVBS_12MHZ) || defined(CVBS_13_5MHZ) || defined(VGA_MODE)
#include "ZxScanlineVgaRenderLoop.h"
#ifndef DEFAULT_VIDEO_DRIVER
#define DEFAULT_VIDEO_DRIVER zx_spectrum_video_driver_vga_index
#endif
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

zx_spectrum_video_driver_t *getZxSpectrumVideoDriver(zx_spectrum_video_driver_enum_t index) {
  return &_zx_spectrum_video_drivers[index];
}

void setZxSpectrumVideoDriver(zx_spectrum_video_driver_enum_t index) {
  _video_driver_index = index;
}

zx_spectrum_video_driver_enum_t videoDriverIndex() {
  return _video_driver_index;
}

const char* videoDriverName() {
  return _zx_spectrum_video_drivers[_video_driver_index].name;
}

void ZxSpectrumVideoNext() {
  for (int i =  0; i < ZX_SPECTRUM_VIDEO_DRIVER_COUNT - 1; ++i) {
    const int k = (1 + i + (int)_video_driver_index) % ZX_SPECTRUM_VIDEO_DRIVER_COUNT;
    if (_zx_spectrum_video_drivers[k].init != NULL) {
      _video_driver_index = (zx_spectrum_video_driver_enum_t)k;
    }
  }
}

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
