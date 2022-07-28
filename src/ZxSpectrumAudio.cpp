#include "ZxSpectrumAudio.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

#ifdef PICO_AUDIO_I2S

#include "audio_i2s.pio.h"
#include "hardware/pio.h"
#include "audio_i2s.pio.h"

static void update_pio_frequency(uint32_t sample_freq, PIO audio_pio, uint pio_sm) {
  uint32_t system_clock_frequency = clock_get_hz(clk_sys);
  assert(system_clock_frequency < 0x40000000);
  uint32_t divider = system_clock_frequency * 4 / (sample_freq * 3); // avoid arithmetic overflow
  assert(divider < 0x1000000);
  pio_sm_set_clkdiv_int_frac(audio_pio, pio_sm, divider >> 8u, divider & 0xffu);
}

static void init_is2_audio() {
  gpio_set_function(PICO_AUDIO_I2S_DATA, PICO_AUDIO_I2S_PIO_FUNC);
  gpio_set_function(PICO_AUDIO_I2S_BCLK, PICO_AUDIO_I2S_PIO_FUNC);
  gpio_set_function(PICO_AUDIO_I2S_BCLK + 1, PICO_AUDIO_I2S_PIO_FUNC);
  uint offset = pio_add_program(PICO_AUDIO_I2S_PIO, &audio_i2s_program);
  audio_i2s_program_init(PICO_AUDIO_I2S_PIO, PICO_AUDIO_I2S_SM, offset, PICO_AUDIO_I2S_DATA, PICO_AUDIO_I2S_BCLK);
  update_pio_frequency(96000, PICO_AUDIO_I2S_PIO, PICO_AUDIO_I2S_SM);
  pio_sm_set_enabled(PICO_AUDIO_I2S_PIO, PICO_AUDIO_I2S_SM, true);
}

inline bool is2_audio_ready() {
  return !pio_sm_is_tx_fifo_full(PICO_AUDIO_I2S_PIO, PICO_AUDIO_I2S_SM);
}

inline void is2_audio_put(uint32_t x) {
  *(volatile uint32_t*)&PICO_AUDIO_I2S_PIO->txf[PICO_AUDIO_I2S_SM] = x;
}

#else
  #ifdef BZR_PIN
    #ifdef AY8912_A_PIN
      #define PWM_WRAP (255)
    #else
      #define PWM_WRAP (255 + 255 + 255)
    #endif
  #else
    #define PWM_WRAP (255 + 255 + 255 + 255)
  #endif

static void init_pwm_pin(uint32_t pin) { 
  gpio_set_function(pin, GPIO_FUNC_PWM);
  const int audio_pin_slice = pwm_gpio_to_slice_num(pin);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 1.0f); 
  pwm_config_set_wrap(&config, PWM_WRAP);
  pwm_init(audio_pin_slice, &config, true);
}
#endif

void zxSpectrumAudioInit() {
#ifdef PICO_AUDIO_I2S
  init_is2_audio();
#else  
  #ifdef BZR_PIN
    gpio_init(BZR_PIN);
    gpio_set_dir(BZR_PIN, GPIO_OUT);
  #endif
  #ifdef SPK_PIN
    init_pwm_pin(SPK_PIN);
  #endif
  #ifdef AY8912_A_PIN
    init_pwm_pin(AY8912_A_PIN);
  #endif
  #ifdef AY8912_B_PIN
    init_pwm_pin(AY8912_B_PIN);
  #endif
  #ifdef AY8912_C_PIN
    init_pwm_pin(AY8912_C_PIN);
  #endif
#endif
}

void zxSpectrumAudioHandler(uint32_t vA, uint32_t vB, uint32_t vC, uint32_t s, uint32_t buzzer) {
#ifdef PICO_AUDIO_I2S
  if (!is2_audio_ready()) return;
  uint32_t l = ((((vA << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1))) & 0xffff;
  uint32_t r = ((((vC << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1))) & 0xffff;
  is2_audio_put((l << 16) | r);
#else
  #ifdef BZR_PIN
    gpio_put(BZR_PIN, buzzer);
    #ifdef SPK_PIN        
      pwm_set_gpio_level(SPK_PIN, vA + vB + vC);
    #else
      pwm_set_gpio_level(AY8912_A_PIN, vA);
      pwm_set_gpio_level(AY8912_B_PIN, vB);
      pwm_set_gpio_level(AY8912_C_PIN, vC);
    #endif
  #else
      pwm_set_gpio_level(SPK_PIN, vA + vB + vC + s);
  #endif
#endif
}

