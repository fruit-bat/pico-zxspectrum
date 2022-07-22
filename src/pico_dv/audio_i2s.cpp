#include <stdio.h>

#include "audio_i2s.pio.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "audio_i2s.h"
#include "audio_i2s.pio.h"

static void update_pio_frequency(uint32_t sample_freq, PIO audio_pio, uint pio_sm) {
    uint32_t system_clock_frequency = clock_get_hz(clk_sys);
    assert(system_clock_frequency < 0x40000000);
    uint32_t divider = system_clock_frequency * 4 / sample_freq; // avoid arithmetic overflow
    assert(divider < 0x1000000);
    pio_sm_set_clkdiv_int_frac(audio_pio, pio_sm, divider >> 8u, divider & 0xffu);
}

void init_is2_audio() {
  
  PIO pio = pio1;
  uint sm = 2;
  uint data_pin = 26;
  uint clock_pin_base = 27;
  
  gpio_set_function(data_pin, GPIO_FUNC_PIO1);
  gpio_set_function(clock_pin_base, GPIO_FUNC_PIO1);
  gpio_set_function(clock_pin_base + 1, GPIO_FUNC_PIO1);
  
  // get a state machine
  // sm = pio_claim_unused_sm(pio, true);
  
  uint offset = pio_add_program(pio, &audio_i2s_program);

  audio_i2s_program_init(pio, sm, offset, data_pin, clock_pin_base);
  update_pio_frequency(96000, pio, sm);
  
  pio_sm_set_enabled(pio, sm, true);
}
