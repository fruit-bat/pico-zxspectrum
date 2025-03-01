#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"

void ZxDviRenderLoopInit();

void ZxDviRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    bool &toggleMenu);
