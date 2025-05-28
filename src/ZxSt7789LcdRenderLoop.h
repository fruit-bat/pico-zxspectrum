#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"
#include "ZxSpectrumMenu.h"
#include "ZxRenderLoopCallbacks.h"

#ifndef LCD_SZ_FRAME_X
#define LCD_SZ_FRAME_X 0
#endif
#ifndef LCD_SZ_FRAME_Y
#define LCD_SZ_FRAME_Y 0
#endif
#ifndef LCD_SZ_FRAME_COLS
#define LCD_SZ_FRAME_COLS 40
#endif
#ifndef LCD_SZ_FRAME_ROWS
#define LCD_SZ_FRAME_ROWS 30
#endif

void ZxSt7789LcdRenderLoopInit();

void ZxSt7789LcdRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin);

zx_spectrum_audio_driver_enum_t ZxSt7789LcdRenderAudioDefault();
