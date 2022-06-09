#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumPicomputerJoystick : public ZxSpectrumJoystick {
private:
  bool _enabled;
public:
  ZxSpectrumPicomputerJoystick();
  virtual uint8_t __not_in_flash_func(kempston)();
  virtual uint8_t __not_in_flash_func(sinclairL)();
  virtual uint8_t __not_in_flash_func(sinclairR)();
  void enabled(bool enabled) { _enabled = enabled; }
};
