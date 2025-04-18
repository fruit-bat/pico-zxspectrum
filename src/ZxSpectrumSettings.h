#pragma once

#include <stdlib.h>
#include "ZxSpectrumJoystick.h"
#include "ZxSpectrumMouse.h"
#include "ZxSpectrumAudioDriver.h"

typedef struct {
  uint32_t volume;
  ZxSpectrumJoystickMode joystickMode;
  ZxSpectrumMouseMode mouseMode;
  ZxSpectrumJoystickMode mouseJoystickMode;
  zx_spectrum_audio_driver_enum_t audioDriverLcdDefault;
  zx_spectrum_audio_driver_enum_t audioDriverVgaDefault;
  zx_spectrum_audio_driver_enum_t audioDriverDviDefault;
} ZxSpectrumSettingValues;

class ZxSpectrumSettings {
private:
  void sanitise(ZxSpectrumSettingValues *values);
protected:
  virtual bool onSave(ZxSpectrumSettingValues *values);
  virtual bool onLoad(ZxSpectrumSettingValues *values);
public:
  ZxSpectrumSettings();
  virtual ~ZxSpectrumSettings();
  bool save(ZxSpectrumSettingValues *values);
  bool load(ZxSpectrumSettingValues *values);
  void defaults(ZxSpectrumSettingValues *values);
};
