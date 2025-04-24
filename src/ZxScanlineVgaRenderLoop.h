#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"
#include "ZxRenderLoopCallbacks.h"
#include "ZxSpectrumMenu.h"

void ZxScanlineVgaRenderLoopInit_CVBS_50Hz_13_5Mhz();
void ZxScanlineVgaRenderLoopInit_CVBS_50Hz_12Mhz();
void ZxScanlineVgaRenderLoopInit_640x480p_60hz();
void ZxScanlineVgaRenderLoopInit_720x576p_50hz();

void ZxScanlineVgaRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin);
