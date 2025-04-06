
#include "ZxSpectrumAudioI2s.h"

#if !defined(PICO_AUDIO_I2S)
#include "ZxSpectrumAudioNull.h"

void i2s_audio_init()
{
  null_audio_init();
}

uint32_t i2s_audio_freq()
{
  return null_audio_freq();
}

void __not_in_flash_func(i2s_audio_handler)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute)
{
  null_audio_handler(vA, vB, vC, s, buzzer, mute);
}

bool __not_in_flash_func(i2s_audio_ready)()
{
  return null_audio_ready();
}

#else

#include "audio_i2s.pio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ZxSpectrumAudioVol.h"
#include "ZxSpectrumAudio16BitStereo.h"

#ifndef PICO_I2S_AUDIO_FREQ
#define PICO_I2S_AUDIO_FREQ 44100
#endif
#ifndef PICO_AUDIO_I2S_PIO
#define PICO_AUDIO_I2S_PIO pio1
#endif
#ifndef PICO_AUDIO_I2S_PIO_FUNC
#define PICO_AUDIO_I2S_PIO_FUNC GPIO_FUNC_PIO1
#endif
#ifndef PICO_AUDIO_I2S_DATA
#define PICO_AUDIO_I2S_DATA 9
#endif
#ifndef PICO_AUDIO_I2S_BCLK
#define PICO_AUDIO_I2S_BCLK 10
#endif

static void update_pio_frequency(uint32_t sample_freq, PIO audio_pio, uint pio_sm)
{
  uint32_t system_clock_frequency = clock_get_hz(clk_sys);
  assert(system_clock_frequency < 0x40000000);
  uint32_t divider = system_clock_frequency * 4 / (sample_freq * 3); // avoid arithmetic overflow
  assert(divider < 0x1000000);
  pio_sm_set_clkdiv_int_frac(audio_pio, pio_sm, divider >> 8u, divider & 0xffu);
}

static uint i2s_audio_sm = 0;

inline bool is2_audio_ready()
{
  return !pio_sm_is_tx_fifo_full(PICO_AUDIO_I2S_PIO, i2s_audio_sm);
}

static inline void is2_audio_put(uint32_t x)
{
  *(volatile uint32_t *)&PICO_AUDIO_I2S_PIO->txf[i2s_audio_sm] = x;
}

void i2s_audio_init()
{
  gpio_set_function(PICO_AUDIO_I2S_DATA, PICO_AUDIO_I2S_PIO_FUNC);
  gpio_set_function(PICO_AUDIO_I2S_BCLK, PICO_AUDIO_I2S_PIO_FUNC);
  gpio_set_function(PICO_AUDIO_I2S_BCLK + 1, PICO_AUDIO_I2S_PIO_FUNC);
  uint offset = pio_add_program(PICO_AUDIO_I2S_PIO, &audio_i2s_program);
#ifdef PICO_AUDIO_I2S_SM
  i2s_audio_sm = PICO_AUDIO_I2S_SM;
#else
  i2s_audio_sm = pio_claim_unused_sm(PICO_AUDIO_I2S_PIO, true);
#endif
  audio_i2s_program_init(PICO_AUDIO_I2S_PIO, i2s_audio_sm, offset, PICO_AUDIO_I2S_DATA, PICO_AUDIO_I2S_BCLK);
  update_pio_frequency(PICO_I2S_AUDIO_FREQ, PICO_AUDIO_I2S_PIO, i2s_audio_sm);
  pio_sm_set_enabled(PICO_AUDIO_I2S_PIO, i2s_audio_sm, true);
}

uint32_t i2s_audio_freq() {
  return PICO_I2S_AUDIO_FREQ;
}

static inline bool audio_ready() {
  return !pio_sm_is_tx_fifo_full(PICO_AUDIO_I2S_PIO, i2s_audio_sm);
}

void __not_in_flash_func(i2s_audio_handler)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute)
{
  uint32_t ll, rr;
  audio_handler_16bit_helper(vA, vB, vC, s, buzzer, mute, ll, rr);

  if (audio_ready())
  {
    is2_audio_put((ll << 16) | rr);
  }
}

bool __not_in_flash_func(i2s_audio_ready)()
{
  return audio_ready();
}

#endif
