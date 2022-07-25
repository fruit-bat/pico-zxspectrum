#pragma once

#include "ZxSpectrum.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
//
// PICO_AUDIO_I2S I2S audio
//
// SPK_PIN   combined audio from AY-3-8912 and buzzer
//
// BZR_PIN   buzzer pin from ZX spectrum, if used mixed audio from 
//           AY-3-8912 will be present on SPK_PIN, unless...
// 
// AY8912_A_PIN
// AY8912_B_PIN
// AY8912_C_PIN
//             
//           Split output from AY-3-8912 
//
// See CMakeLists.txt files for configurations
//
#ifdef PICO_AUDIO_I2S
#include "hardware/pio.h"

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
#endif

void zxSpectrumAudioInit();

inline bool zxSpectrumAudioReady() {
#ifdef PICO_AUDIO_I2S
  return is2_audio_ready();
#else
  return true;
#endif
}

inline void zxSpectrumAudioToGpio(ZxSpectrum& zxSpectrum) {
#ifdef PICO_AUDIO_I2S
  uint32_t vA, vB, vC;
  zxSpectrum.vol(vA, vB, vC);
  uint32_t s = zxSpectrum.getBuzzer() ? 255 : 0;
  uint32_t l = ((((vA << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1))) & 0xffff;
  uint32_t r = ((((vC << 1) + vB + s) << 4) - ((255 + 255 + 255 + 255) << (4 - 1))) & 0xffff;
  is2_audio_put((l << 16) | r);
#else
  #ifdef BZR_PIN
        gpio_put(BZR_PIN, zxSpectrum.getBuzzer());
    #ifdef SPK_PIN
          const uint32_t l = zxSpectrum.getAnalogueAudio();
          pwm_set_gpio_level(SPK_PIN, l);
    #else
          uint32_t vA, vB, vC;
          zxSpectrum.vol(vA, vB, vC);
          pwm_set_gpio_level(AY8912_A_PIN, vA);
          pwm_set_gpio_level(AY8912_B_PIN, vB);
          pwm_set_gpio_level(AY8912_C_PIN, vC);
    #endif
  #else
        const uint32_t l = zxSpectrum.getSpeaker();
        pwm_set_gpio_level(SPK_PIN, l);
  #endif
#endif
}

