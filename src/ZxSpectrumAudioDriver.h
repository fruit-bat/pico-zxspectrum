#pragma once

#include <pico/stdlib.h>
#include "ZxSpectrumAudioDriverEnum.h"

typedef void (*zx_spectrum_audio_init_t)();
typedef uint32_t (*zx_spectrum_audio_freq_t)();
typedef void (*zx_spectrum_audio_handler_t)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t buzzerSmoothed, uint32_t buzzer, bool mute);
typedef bool (*zx_spectrum_audio_ready_t)();

typedef struct
{
  zx_spectrum_audio_init_t init;
  zx_spectrum_audio_freq_t freq;
  zx_spectrum_audio_ready_t ready;
  zx_spectrum_audio_handler_t handler;
  const char *name;
} zx_spectrum_audio_driver_t;

extern zx_spectrum_audio_driver_t _zx_spectrum_audio_drivers[];

void zx_spectrum_audio_driver_init();

zx_spectrum_audio_driver_enum_t zx_spectrum_audio_driver_index(zx_spectrum_audio_driver_t *d);
