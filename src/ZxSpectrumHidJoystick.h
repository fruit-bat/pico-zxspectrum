#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumHidJoystick : public ZxSpectrumJoystick {
  
public:
  virtual uint8_t __not_in_flash_func(kempston)();
};
