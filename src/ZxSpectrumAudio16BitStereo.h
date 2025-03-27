#pragma once

#include <pico/stdlib.h>

inline void audio_handler_16bit_helper(
  const uint32_t vA, 
  const uint32_t vB,
  const uint32_t vC, 
  const int32_t s, 
  const uint32_t buzzer, 
  const bool mute, 
  uint32_t& ll, 
  uint32_t& rr)
{
  if (mute)
  {
    ll = rr = 0;
  }
  else
  {
    const int32_t l = (vA << 1) + vB + s - (128 * 3);
    const int32_t r = (vC << 1) + vB + s - (128 * 3);
    const int32_t v = __mul_instruction(_vol, 60);
    ll = (__mul_instruction(v, l) >> 8) & 0xffff;
    rr = (__mul_instruction(v, r) >> 8) & 0xffff;
  }
}