#pragma once

#include <pico/stdlib.h>

void i2s_audio_init();

uint32_t i2s_audio_freq();

void i2s_audio_handler(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute);

bool i2s_audio_ready();
