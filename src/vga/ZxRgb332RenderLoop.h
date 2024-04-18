#pragma once

#include "ZxSpectrum.h"

void ZxRgb332RenderLoopInit();

void ZxRgb332RenderLoop(
    ZxSpectrum &zxSpectrum,
    uint &frames,
    bool &showMenu,
    bool &toggleMenu);

extern void ZxRgb332RenderLoopCallbackLine(uint32_t y);
extern void ZxRgb332RenderLoopCallbackMenu(bool state);
