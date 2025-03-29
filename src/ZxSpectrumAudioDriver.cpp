#include "ZxSpectrumAudioDriver.h"
#include "ZxSpectrumAudioPioPwm.h"
#include "ZxSpectrumAudioPwm.h"
#include "ZxSpectrumAudioHdmi.h"
#include "ZxSpectrumAudioNull.h"
#include "ZxSpectrumAudioI2s.h"

zx_spectrum_audio_driver_t _zx_spectrum_audio_drivers[] = {
  {
    null_audio_init,
    null_audio_ready,
    null_audio_handler
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
