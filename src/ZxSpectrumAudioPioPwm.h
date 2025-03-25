#pragma once

void init_pio_pwm_audio();

void pio_pwm_audio_handler(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute, uint32_t vol);

bool pio_pwm_audio_ready();
