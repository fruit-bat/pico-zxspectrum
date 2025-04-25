#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"
#include "ZxSpectrumMenu.h"
#include "ZxRenderLoopCallbacks.h"

typedef void (*zx_spectrum_video_init_t)();

typedef void (*zx_spectrum_video_loop_t)(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin
);

void ZxSpectrumVideoInit();

void ZxSpectrumVideoLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin
);

typedef struct
{
    zx_spectrum_video_init_t init;
    zx_spectrum_video_loop_t loop;
    const char *name;
} zx_spectrum_video_driver_t;

typedef enum
{
    zx_spectrum_video_driver_vga_index = 0,
    zx_spectrum_video_driver_dvi_index,
    zx_spectrum_video_driver_lcd_index,
} zx_spectrum_video_driver_enum_t;

#define ZX_SPECTRUM_VIDEO_DRIVER_COUNT (zx_spectrum_video_driver_lcd_index + 1)

zx_spectrum_video_driver_enum_t videoDriverIndex();

const char* videoDriverName();
