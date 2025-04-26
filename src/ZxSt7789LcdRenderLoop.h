#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"
#include "ZxSpectrumMenu.h"
#include "ZxRenderLoopCallbacks.h"

void ZxSt7789LcdRenderLoopInit();

void ZxSt7789LcdRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin);

zx_spectrum_audio_driver_enum_t ZxSt7789LcdRenderAudioDefault();
