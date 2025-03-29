#pragma once

#include <pico/stdlib.h>

typedef uint32_t (*zx_spectrum_audio_init_t)();
typedef void (*zx_spectrum_audio_handler_t)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t buzzerSmoothed, uint32_t buzzer, bool mute);
typedef bool (*zx_spectrum_audio_ready_t)();

typedef struct
{
  zx_spectrum_audio_init_t init;
  zx_spectrum_audio_ready_t ready;
  zx_spectrum_audio_handler_t handler;
} zx_spectrum_audio_driver_t;

extern zx_spectrum_audio_driver_t _zx_spectrum_audio_drivers[];

typedef enum zx_spectrum_audio_driver_enum
{
  zx_spectrum_audio_driver_null_index = 0,
  zx_spectrum_audio_driver_pio_pwm_index,
  zx_spectrum_audio_driver_pwm_index,
  zx_spectrum_audio_driver_i2s_index,
  zx_spectrum_audio_driver_hdmi_index
} zx_spectrum_audio_driver_enum_t;

#if !defined(PICO_DEFAULT_AUDIO)
#if defined(PICO_HDMI_AUDIO)
#define PICO_DEFAULT_AUDIO zx_spectrum_audio_driver_hdmi_index
#elif defined(PICO_AUDIO_I2S)
#define PICO_DEFAULT_AUDIO zx_spectrum_audio_driver_i2s_index
#elif defined(PICO_PIO_PWM_AUDIO)
#define PICO_DEFAULT_AUDIO zx_spectrum_audio_driver_pio_pwm_index
#else
#define PICO_DEFAULT_AUDIO zx_spectrum_audio_driver_pwm_index
#endif
#endif
