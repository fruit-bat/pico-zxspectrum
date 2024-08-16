#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"

void ZxScanlineVgaRenderLoopInit();

void ZxScanlineVgaRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    bool &toggleMenu);

extern void ZxScanlineVgaRenderLoopCallbackLine(uint32_t y);
extern void ZxScanlineVgaRenderLoopCallbackMenu(bool state);
