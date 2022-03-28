#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumHidJoystick : public ZxSpectrumJoystick {
private:
  uint32_t _updatedL;
  uint32_t _updatedR;
  uint8_t _kempston;
  uint8_t _sinclairL;
  uint8_t _sinclairR;
public:
  ZxSpectrumHidJoystick();
  void __not_in_flash_func(decode)();
  virtual uint8_t __not_in_flash_func(kempston)();
  virtual uint8_t __not_in_flash_func(sinclairL)();
  virtual uint8_t __not_in_flash_func(sinclairR)();  
};
