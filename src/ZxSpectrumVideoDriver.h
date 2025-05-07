#pragma once

#include "ZxSpectrum.h"
#include "ZxSpectrumDisplay.h"
#include "ZxSpectrumMenu.h"
#include "ZxRenderLoopCallbacks.h"
#include "ZxSpectrumVideoDriverEnum.h"
#include "ZxSpectrumAudioDriverEnum.h"

typedef void (*zx_spectrum_video_init_t)();

typedef void (*zx_spectrum_video_loop_t)(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin
);

typedef zx_spectrum_audio_driver_enum_t (*zx_spectrum_video_audio_default_t)();

void ZxSpectrumVideoInit();

void ZxSpectrumVideoLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin
);

typedef struct
{
    zx_spectrum_video_init_t init;
    zx_spectrum_video_loop_t loop;
    zx_spectrum_video_audio_default_t audio_default;
    const char *name;
} zx_spectrum_video_driver_t;

zx_spectrum_video_driver_enum_t videoDriverIndex();

const char* videoDriverName();

void ZxSpectrumVideoNext();

zx_spectrum_video_driver_t *getZxSpectrumVideoDriver(zx_spectrum_video_driver_enum_t index);

void setZxSpectrumVideoDriver(zx_spectrum_video_driver_enum_t index);

inline bool isZxSpectrumVideoDriverInstalled(zx_spectrum_video_driver_enum_t index) {
    return getZxSpectrumVideoDriver(index)->init != NULL;
}
