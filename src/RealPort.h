#pragma once

#if defined(PICO_MITE_PORT)
#include "PicoMitePort.h"
static PicoMitePort picoMitePort;
#define ZX_SPECTRUM_REAL_PORT &picoMitePort
#define ZX_SPECTRUM_REAL_PORT_INIT picoMitePort.init();
#elif defined(WAVESHARE_PI_ZERO_PORT)
#include "WavesharePiZeroPort.h"
static WavesharePiZeroPort wavesharePiZeroPort;
#define ZX_SPECTRUM_REAL_PORT &wavesharePiZeroPort
#define ZX_SPECTRUM_REAL_PORT_INIT wavesharePiZeroPort.init();
#else
#define ZX_SPECTRUM_REAL_PORT 0
#define ZX_SPECTRUM_REAL_PORT_INIT
#endif

