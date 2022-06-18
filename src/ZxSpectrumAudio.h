#pragma once

#include "ZxSpectrum.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

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


// #define SPK_PIN 20
// #define AY8912_A_PIN 20
// #define AY8912_B_PIN 26
// #define AY8912_C_PIN 27

#ifdef BZR_PIN
	#ifdef AY8912_A_PIN
		#define PWM_WRAP (255)
	#else
		#define PWM_WRAP (255 + 255 + 255)
	#endif
#else
  #define PWM_WRAP (255 + 255 + 255 + 255)
#endif

void zxSpectrumAudioInit();

inline void zxSpectrumAudioToGpio(ZxSpectrum& zxSpectrum) {
  
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
}

