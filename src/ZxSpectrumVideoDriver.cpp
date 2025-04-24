#include "ZxSpectrumVideoDriver.h"

#include "ZxDviRenderLoop.h"
#include "ZxSt7789LcdRenderLoop.h"

#if defined(CVBS_VIDEO) || defined(VGA_VIDEO)
#include "ZxScanlineVgaRenderLoop.h"
#else
#define ZxScanlineVgaRenderLoopInit_640x480p_60hz NULL
#define ZxScanlineVgaRenderLoopInit_720x576p_50hz NULL
#define ZxScanlineVgaRenderLoopInit_CVBS_50Hz_13_5Mhz NULL
#define ZxScanlineVgaRenderLoopInit_CVBS_50Hz_12Mhz NULL
#define ZxScanlineVgaRenderLoop NULL
#endif

zx_spectrum_video_driver_t _zx_spectrum_video_drivers[ZX_SPECTRUM_VIDEO_DRIVER_COUNT] = {  
  {
    ZxScanlineVgaRenderLoopInit_CVBS_50Hz_13_5Mhz,
    ZxScanlineVgaRenderLoop,
    "CVBS 50Hz 13.5Mhz"
  },
  {
    ZxScanlineVgaRenderLoopInit_CVBS_50Hz_12Mhz,
    ZxScanlineVgaRenderLoop,
    "CVBS 50Hz 12Mhz"
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

static zx_spectrum_video_driver_enum_t _video_driver_index = 
#if defined(DVI_DEFAULT_SERIAL_CONFIG)
zx_spectrum_video_driver_dvi_640x480p_60hz_index
#elif defined(PICO_ST7789_LCD)
zx_spectrum_video_driver_lcd_index
#elif defined(VGA_MODE)
zx_spectrum_video_driver_vga_640x480p_60hz_index
#else
zx_spectrum_video_driver_null_index
#endif
;

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
