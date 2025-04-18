#include "ZxSpectrumSettings.h"
#include "ZxSpectrumAudio.h"

ZxSpectrumSettings::ZxSpectrumSettings()
{
}

ZxSpectrumSettings::~ZxSpectrumSettings()
{
}

inline static bool is_audio_driver_valid(uint i) {
  return i < ZX_SPECTRUM_AUDIO_DRIVER_COUNT;
}

inline static zx_spectrum_audio_driver_enum_t non_dvi_audio_default() {
#if defined(PICO_AUDIO_I2S)
  return zx_spectrum_audio_driver_i2s_index;
#elif defined(PICO_PIO_PWM_AUDIO)      
  return zx_spectrum_audio_driver_pio_pwm_index;
#elif defined(PICO_PWM_AUDIO)      
  return zx_spectrum_audio_driver_pwm_index;
#else
  return zx_spectrum_audio_driver_null_index;
#endif  
}

void ZxSpectrumSettings::sanitise(ZxSpectrumSettingValues *values) {

    // Volume
    if (values->volume > 0x100) values->volume = 0x100;

    // Joystick
    switch(values->joystickMode) {
      case ZxSpectrumJoystickModeKempston:
      case ZxSpectrumJoystickModeSinclairLR:
      case ZxSpectrumJoystickModeSinclairRL:
        break;
      default: 
        values->joystickMode = ZxSpectrumJoystickModeKempston;
        break;
    }

    // Mouse
    switch(values->mouseMode) {
      case ZxSpectrumMouseModeKempstonMouse:
      case ZxSpectrumMouseModeJoystick:
        break;
      default: 
        values->mouseMode = ZxSpectrumMouseModeKempstonMouse;
        break;
    } 

    // Mouse Joystick
    switch(values->mouseJoystickMode) {
      case ZxSpectrumJoystickModeKempston:
      case ZxSpectrumJoystickModeSinclairLR:
      case ZxSpectrumJoystickModeSinclairRL:
        break;
      default: 
        values->mouseJoystickMode = ZxSpectrumJoystickModeKempston;
        break;
    }

    // Audio LCD default
    if (!is_audio_driver_valid(values->audioDriverLcdDefault)) {
      values->audioDriverLcdDefault = non_dvi_audio_default();
    } 

    // Audio DVI default
    if (!is_audio_driver_valid(values->audioDriverDviDefault)) {
      values->audioDriverDviDefault = zx_spectrum_audio_driver_hdmi_index;
    }

    // Audio VGA default
    if (!is_audio_driver_valid(values->audioDriverVgaDefault)) {
      values->audioDriverVgaDefault = non_dvi_audio_default();
    } 
}

bool ZxSpectrumSettings::onSave(ZxSpectrumSettingValues *values)
{
    return false;
}

bool ZxSpectrumSettings::onLoad(ZxSpectrumSettingValues *values)
{
    return false;
}

bool ZxSpectrumSettings::save(ZxSpectrumSettingValues *values)
{
    sanitise(values);
    return onSave(values);
}

bool ZxSpectrumSettings::load(ZxSpectrumSettingValues *values)
{
    defaults(values);
    bool r = onLoad(values);
    sanitise(values);
    return r;
}

void ZxSpectrumSettings::defaults(ZxSpectrumSettingValues *values) {
  values->volume = 0x100;
  values->joystickMode = ZxSpectrumJoystickModeKempston;
  values->mouseMode = ZxSpectrumMouseModeKempstonMouse;
}
