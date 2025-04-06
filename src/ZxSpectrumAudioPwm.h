#pragma once

#include <pico/stdlib.h>

// Default to hardware PWM
// TODO This is messy - force a choice in makefile
#if !defined(PICO_PIO_PWM_AUDIO) && !defined(PICO_AUDIO_I2S) && !defined(PICO_HDMI_AUDIO)
#define PICO_PWM_AUDIO
#endif

void pwm_audio_init();

uint32_t pwm_audio_freq();

void pwm_audio_handler(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute);

bool pwm_audio_ready();
