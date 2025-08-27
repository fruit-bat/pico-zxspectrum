#include "ZxSpectrumAudioPwm.h"

#if !defined(PICO_PWM_AUDIO)
#include "ZxSpectrumAudioNull.h"

void pwm_audio_init()
{
  null_audio_init();
}

uint32_t pwm_audio_freq()
{
  return null_audio_freq();
}

void __not_in_flash_func(pwm_audio_handler)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute)
{
  null_audio_handler(vA, vB, vC, s, buzzer, mute);
}

bool __not_in_flash_func(pwm_audio_ready)()
{
  return null_audio_ready();
}

#else

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/printf.h"
#include "pwm.pio.h"

#include "ZxSpectrumAudioPwm.h"
#include "ZxSpectrumAudioBuf.h"
#include "ZxSpectrumAudioVol.h"

#ifndef PICO_PWM_AUDIO_FREQ
#define PICO_PWM_AUDIO_FREQ 28000
#endif

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

// TODO we don't always want this memeory used up!
static zx_audio_buf_t zx_audio_buf;
static struct repeating_timer timer;

bool __not_in_flash_func(repeating_timer_callback)(struct repeating_timer *timer)
{
  static zx_audio_sample_t last;
  uint32_t vA;
  uint32_t vB;
  uint32_t vC;
  int32_t s;
  uint32_t buzzer;
  
  if (zx_audio_buf_ready_for_read(&zx_audio_buf))
  {
    zx_audio_sample_t *buf = zx_audio_buf_read_ptr(&zx_audio_buf);
    last = *buf;
    vA = buf->vA;
    vB = buf->vB;
    vC = buf->vC;
    s = buf->s;
    buzzer = buf->b;
    zx_audio_buf_read_next(&zx_audio_buf);
  }
  else {
    vA = last.vA;
    vB = last.vB;
    vC = last.vC;
    s = last.s;
    buzzer = last.b;
  }
#ifdef BZR_PIN
    gpio_put(BZR_PIN, buzzer);
#ifdef SPK_PIN
    pwm_set_gpio_level(SPK_PIN, vA + vB + vC);
#else
#ifdef AY8912_ABC_STEREO
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
    uint32_t st = __mul_instruction(_vol, s) >> 8;
    pwm_set_gpio_level(AY8912_A_PIN, lt);
    pwm_set_gpio_level(AY8912_C_PIN, rt);
    pwm_set_gpio_level(SPK_PIN, st);
#else 
    uint32_t ayt = __mul_instruction(_vol, vA + vB + vC) >> 8;
    uint32_t ss = __mul_instruction(_vol, s) >> 8;
    uint32_t t = ayt + ss + 128;
    pwm_set_gpio_level(SPK_PIN, t >= 255 + 255 + 255 ? ayt - ss : t);
#endif
#endif
  return true;
}

static void init_audio_output_timer() {
    zx_audio_buf_init(&zx_audio_buf);
    add_repeating_timer_us(-(1000000/PICO_PWM_AUDIO_FREQ), repeating_timer_callback, NULL, &timer);
}

static void init_pwm_pin(uint32_t pin) { 
    gpio_set_function(pin, GPIO_FUNC_PWM);
    const int audio_pin_slice = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f); 
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_init(audio_pin_slice, &config, true);
}

void pwm_audio_init() {

    init_audio_output_timer();
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
}

uint32_t pwm_audio_freq() {
    return PICO_PWM_AUDIO_FREQ;
}

void __not_in_flash_func(pwm_audio_handler)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute) {
  zx_audio_sample_t *buf = zx_audio_buf_write_ptr(&zx_audio_buf);
  if (mute) {
    buf->vA = 0;
    buf->vB = 0;
    buf->vC = 0;
    buf->s = 0;
    buf->b = 0;
  }
  else {
    buf->vA = vA;
    buf->vB = vB;
    buf->vC = vC;
    buf->s = s;
    buf->b = buzzer;
  }
  zx_audio_buf_write_next(&zx_audio_buf);
}

bool __not_in_flash_func(pwm_audio_ready)() {
  return zx_audio_buf_ready_for_write(&zx_audio_buf);
}

#endif