#pragma once

#include <pico/stdlib.h>

typedef enum
{
  zx_spectrum_audio_driver_null_index = 0,
  zx_spectrum_audio_driver_pio_pwm_index,
  zx_spectrum_audio_driver_pwm_index,
  zx_spectrum_audio_driver_i2s_index,
  zx_spectrum_audio_driver_hdmi_index
} zx_spectrum_audio_driver_enum_t;

#define ZX_SPECTRUM_AUDIO_DRIVER_COUNT (zx_spectrum_audio_driver_hdmi_index + 1)
