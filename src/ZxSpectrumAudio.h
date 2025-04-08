#pragma once

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "ZxSpectrumAudioDriver.h"

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

zx_spectrum_audio_driver_t* zxSpectrumAudioInit(zx_spectrum_audio_driver_enum_t audio_driver_index);

uint32_t zxSpectrumReadEar();
bool zxSpectrumEarReady();
