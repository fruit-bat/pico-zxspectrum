#include "ZxSpectrumAudio.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "dvi.h"

#ifndef INITIAL_VOL
#define INITIAL_VOL 0x010
#endif
static uint32_t _vol = INITIAL_VOL;

#if defined(PICO_HDMI_AUDIO)
extern struct dvi_inst dvi0;
#endif

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
      #ifdef AY8912_ABC_STEREO
        #define PWM_WRAP (255 + 255)
      #else
        #define PWM_WRAP (255)
      #endif
    #else
      #define PWM_WRAP (255 + 255 + 255)
    #endif
  #else
    #define PWM_WRAP (255 + 255 + 255)
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

#ifdef EAR_PIN
#include "zx_ear_in.pio.h"

static  PIO ear_pio = pio1;
static  uint ear_sm = 0;

static void init_ear_in() {
  uint offset = pio_add_program(ear_pio, &zx_ear_in_program);
  ear_sm = pio_claim_unused_sm(ear_pio, true);
  zx_ear_in_program_init(ear_pio, ear_sm, offset, EAR_PIN, 1000000);
}

uint32_t __not_in_flash_func(zxSpectrumReadEar)() {
    while ((ear_pio->fstat & (1u << (PIO_FSTAT_RXEMPTY_LSB + ear_sm))) != 0);
    return ear_pio->rxf[ear_sm];
}

#endif

void zxSpectrumAudioInit() {
#if defined(PICO_HDMI_AUDIO)

#elif defined(PICO_AUDIO_I2S)
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
#ifdef EAR_PIN
  init_ear_in();
#endif
}

void __not_in_flash_func(zxSpectrumAudioHandler)(uint32_t vA, uint32_t vB, uint32_t vC, uint32_t s, uint32_t buzzer) {
#if defined(PICO_HDMI_AUDIO)
  uint32_t l = (((vA << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1));
  uint32_t r = (((vC << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1));
  uint32_t ll = (__mul_instruction(_vol, l) >> 8) & 0xffff;
  uint32_t rr = (__mul_instruction(_vol, r) >> 8) & 0xffff;
	audio_sample_t *audio_ptr = get_write_pointer(&dvi0.audio_ring);
	audio_ptr->channels[0] = ll;
	audio_ptr->channels[1] = rr;
	increase_write_pointer(&dvi0.audio_ring, 1);
#elif defined(PICO_AUDIO_I2S)
  if (!is2_audio_ready()) return;
  uint32_t l = (((vA << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1));
  uint32_t r = (((vC << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1));
  uint32_t ll = (__mul_instruction(_vol, l) >> 8) & 0xffff;
  uint32_t rr = (__mul_instruction(_vol, r) >> 8) & 0xffff;
  is2_audio_put((ll << 16) | rr);
#else
  #ifdef BZR_PIN
    gpio_put(BZR_PIN, buzzer);
    #ifdef SPK_PIN
      pwm_set_gpio_level(SPK_PIN, vA + vB + vC);
    #else
      #ifdef AY8912_ABC_STERO
      pwm_set_gpio_level(AY8912_A_PIN, vA + vB);
      pwm_set_gpio_level(AY8912_C_PIN, vC + vB);
      #else
      pwm_set_gpio_level(AY8912_A_PIN, vA);
      pwm_set_gpio_level(AY8912_B_PIN, vB);
      pwm_set_gpio_level(AY8912_C_PIN, vC);
      #endif
    #endif
  #else
    #ifdef AY8912_ABC_STEREO
    uint32_t lt = __mul_instruction(_vol, vA + vB) >> 8;
    uint32_t rt = __mul_instruction(_vol, vC + vB) >> 8;
    uint32_t st  = __mul_instruction(_vol, s) >> 8;
    pwm_set_gpio_level(AY8912_A_PIN, lt);
    pwm_set_gpio_level(AY8912_C_PIN, rt);
    pwm_set_gpio_level(SPK_PIN, st);
    #else
    uint32_t ayt = __mul_instruction(_vol, vA + vB + vC) >> 8;
    uint32_t ss  = __mul_instruction(_vol, s) >> 8;
    uint32_t t   = ayt + ss;
    pwm_set_gpio_level(SPK_PIN, t >= 255 + 255 + 255 ? ayt - ss : t);
    #endif
  #endif
#endif
}

uint32_t zxSpectrumAudioGetVolume() { return _vol; }

void zxSpectrumAudioSetVolume(uint32_t vol) { _vol = vol; }

bool __not_in_flash_func(zxSpectrumAudioReady)() {
#if defined(PICO_HDMI_AUDIO)
  return get_write_size(&dvi0.audio_ring, true) > 0;
#else
  return true;
#endif
}