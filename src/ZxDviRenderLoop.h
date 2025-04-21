#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"
#include "ZxRenderLoopCallbacks.h"
#include "ZxSpectrumMenu.h"

void ZxDviRenderLoopInit();

void ZxDviRenderLoopInit_640x480p_60hz();
void ZxDviRenderLoopInit_720x540p_50hz();
void ZxDviRenderLoopInit_720x576p_50hz();

void ZxDviRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin);
