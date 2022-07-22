#pragma once
#include "hardware/pio.h"

void init_is2_audio();

inline bool is2_audio_ready() {
  PIO pio = pio1;
  uint sm = 2;
  return !pio_sm_is_tx_fifo_full(pio, sm);
}

inline void is2_audio_put(uint32_t x) {
  PIO pio = pio1;
  uint sm = 2;
  *(volatile uint32_t*)&pio->txf[sm] = x;
}
