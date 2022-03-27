#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumHidJoystick : public ZxSpectrumJoystick {
private:
  uint32_t _updated;
  uint8_t _kempston;
public:
  virtual uint8_t __not_in_flash_func(kempston)();
};
