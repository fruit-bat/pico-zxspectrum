#include "ZxSpectrumSettings.h"
#include "ZxSpectrumAudioDriver.h"
#include "ZxSpectrumVideoDriver.h"

ZxSpectrumSettings::ZxSpectrumSettings()
{
}

ZxSpectrumSettings::~ZxSpectrumSettings()
{
}

inline static bool is_audio_driver_valid(uint i) {
  return i < ZX_SPECTRUM_AUDIO_DRIVER_COUNT && i > 0;
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

    // Video output
    if (values->videoDriverDefault >= ZX_SPECTRUM_VIDEO_DRIVER_COUNT || !isZxSpectrumVideoDriverInstalled((zx_spectrum_video_driver_enum_t)values->videoDriverDefault)) {
      values->videoDriverDefault = (uint8_t)videoDriverIndex();
    }

    // Audio output
    for(uint8_t i = 0; i <  ZX_SPECTRUM_VIDEO_DRIVER_COUNT; ++i) {
      if (values->audioDriverDefault[i] >= ZX_SPECTRUM_AUDIO_DRIVER_COUNT) {
        zx_spectrum_video_driver_t* videoDriver = getZxSpectrumVideoDriver((zx_spectrum_video_driver_enum_t)i);
        values->audioDriverDefault[i] = videoDriver 
          ? (uint8_t)videoDriver->audio_default()
          : 0;
      }
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
  values->videoDriverDefault = (uint8_t)videoDriverIndex();
  for(uint8_t i = 0; i <  ZX_SPECTRUM_VIDEO_DRIVER_COUNT; ++i) {
    zx_spectrum_video_driver_t* videoDriver = getZxSpectrumVideoDriver((zx_spectrum_video_driver_enum_t)i);
    values->audioDriverDefault[i] = videoDriver 
      ? (uint8_t)videoDriver->audio_default()
      : 0;
  }
}
