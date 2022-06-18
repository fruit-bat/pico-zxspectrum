#include "ZxSpectrumAudio.h"

static void init_pwm_pin(uint32_t pin) {
  gpio_set_function(pin, GPIO_FUNC_PWM);
  const int audio_pin_slice = pwm_gpio_to_slice_num(pin);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 1.0f); 
  pwm_config_set_wrap(&config, PWM_WRAP);
  pwm_init(audio_pin_slice, &config, true);
}

void zxSpectrumAudioInit() {
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
