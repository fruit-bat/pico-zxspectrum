#include "ZxSpectrumAudioDriver.h"
#include "ZxSpectrumAudioPioPwm.h"
#include "ZxSpectrumAudioPwm.h"
#include "ZxSpectrumAudioI2s.h"
#include "ZxSpectrumAudioHdmi.h"

uint32_t __not_in_flash_func(zx_spectrum_audio_init_null)() { return 280000; }
bool __not_in_flash_func(zx_spectrum_audio_ready_null)() { return true; }
void __not_in_flash_func(zx_spectrum_audio_handler_null)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t buzzerSmoothed, uint32_t buzzer, bool mute) {}

zx_spectrum_audio_driver_t _zx_spectrum_audio_drivers[] = {
  {
    zx_spectrum_audio_init_null,
    zx_spectrum_audio_ready_null,
    zx_spectrum_audio_handler_null
  },
  {
    pio_pwm_audio_init,
    pio_pwm_audio_ready,
    pio_pwm_audio_handler
  },
  {
    pwm_audio_init,
    pwm_audio_ready,
    pwm_audio_handler
  },
  {
    i2s_audio_init,
    i2s_audio_ready,
    i2s_audio_handler
  },
  {
    hdmi_audio_init,
    hdmi_audio_ready,
    hdmi_audio_handler
  }
};
