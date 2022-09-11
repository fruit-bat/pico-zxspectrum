#pragma once

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
void zxSpectrumAudioInit();

void __not_in_flash_func(zxSpectrumAudioHandler)(uint32_t vA, uint32_t vB, uint32_t vC, uint32_t buzzerSmoothed, uint32_t buzzer);

#ifdef EAR_PIN
uint32_t __not_in_flash_func(zxSpectrumReadEar)();
#endif

