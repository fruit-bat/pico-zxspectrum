#pragma once

#include <stdlib.h>
#include "ZxSpectrumJoystick.h"

typedef struct {
  uint32_t volume;
  ZxSpectrumJoystickMode joystickMode;
} ZxSpectrumSettingValues;

class ZxSpectrumSettings {

public:
  ZxSpectrumSettings();
  virtual ~ZxSpectrumSettings();

  virtual bool save(ZxSpectrumSettingValues *values);
  virtual bool load(ZxSpectrumSettingValues *values);
};

