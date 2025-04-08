#pragma once

#include <pico/stdlib.h>

void pio_pwm_audio_init();

uint32_t pio_pwm_audio_freq();

void pio_pwm_audio_handler(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute);

bool pio_pwm_audio_ready();
