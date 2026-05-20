// SPDX-License-Identifier: MIT
// Copyright (c) 2026 fruit-bat
#pragma once

#include <pico/stdlib.h>

extern uint32_t _vol;

inline uint32_t zxSpectrumAudioGetVolume() { return _vol; }

inline void zxSpectrumAudioSetVolume(uint32_t vol) { _vol = vol; }
