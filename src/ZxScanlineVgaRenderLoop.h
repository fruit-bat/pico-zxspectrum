#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"
#include "ZxRenderLoopCallbacks.h"
#include "ZxSpectrumMenu.h"

void ZxScanlineVgaRenderLoopInit();

void ZxScanlineVgaRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin);

zx_spectrum_audio_driver_enum_t ZxScanlineVgaRenderLoopAudioDefault();
