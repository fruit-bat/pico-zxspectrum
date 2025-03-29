#pragma once

#include <pico/stdlib.h>

uint32_t null_audio_init();

void null_audio_handler(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute);

bool null_audio_ready();
