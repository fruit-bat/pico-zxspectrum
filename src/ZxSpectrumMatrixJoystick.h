#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumMatrixJoystick : public ZxSpectrumJoystick {
private:
  bool _enabled;
public:
  ZxSpectrumMatrixJoystick();
  virtual uint8_t __not_in_flash_func(kempston)();
  virtual uint8_t __not_in_flash_func(sinclairL)();
  virtual uint8_t __not_in_flash_func(sinclairR)();
  virtual bool __not_in_flash_func(isConnectedL)() { return true; }
  virtual bool __not_in_flash_func(isConnectedR)() { return false; }  
  void enabled(bool enabled) { _enabled = enabled; }
  virtual uint8_t __not_in_flash_func(joy1)();
};
