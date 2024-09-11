#pragma once

#include <stdlib.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumNespadJoystick : public ZxSpectrumJoystick {
private:
  uint32_t _state; 
  uint8_t _kempston;
  uint8_t _sinclairL;
  uint8_t _sinclairR;
public:
  ZxSpectrumNespadJoystick();
  void __not_in_flash_func(decode)();
  virtual uint8_t __not_in_flash_func(kempston)();
  virtual uint8_t __not_in_flash_func(sinclairL)();
  virtual uint8_t __not_in_flash_func(sinclairR)();
  uint8_t __not_in_flash_func(joy1)();
  virtual bool __not_in_flash_func(isConnectedL)();
  virtual bool __not_in_flash_func(isConnectedR)();
  void init();
};
