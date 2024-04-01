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
#ifndef PICO_PWM_AUDIO_FREQ
#define PICO_PWM_AUDIO_FREQ 28000
#endif
#ifndef PICO_HDMI_AUDIO_FREQ
#define PICO_HDMI_AUDIO_FREQ 44100
#endif
#ifndef PICO_I2S_AUDIO_FREQ
#define PICO_I2S_AUDIO_FREQ 44100
#endif
#if defined(PICO_HDMI_AUDIO)
#define PICO_AUDIO_OUT_FREQ PICO_HDMI_AUDIO_FREQ
#elif defined(PICO_AUDIO_I2S)
#define PICO_AUDIO_OUT_FREQ PICO_I2S_AUDIO_FREQ
#else
#define PICO_AUDIO_OUT_FREQ PICO_PWM_AUDIO_FREQ
#endif

void zxSpectrumAudioInit();

void zxSpectrumAudioHandler(uint32_t vA, uint32_t vB, uint32_t vC, uint32_t buzzerSmoothed, uint32_t buzzer, bool mute);

uint32_t zxSpectrumReadEar();
bool zxSpectrumEarReady();

uint32_t zxSpectrumAudioGetVolume();

void zxSpectrumAudioSetVolume(uint32_t vol);
bool zxSpectrumAudioReady();
