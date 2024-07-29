#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumHidJoystick : public ZxSpectrumJoystick {
private:
  uint32_t _updated1;
  uint32_t _updated2;
  uint8_t _joy1;  //digital joystick1  4button + direction 
  uint8_t _joy2;  //digital joystick2  4button + direction 
  uint8_t _kempston;
  uint8_t _sinclairL;
  uint8_t _sinclairR;
public:
  ZxSpectrumHidJoystick();
  void __not_in_flash_func(decode)();
  uint8_t __not_in_flash_func(joy2sinclair)(uint8_t joy);
  uint8_t __not_in_flash_func(getjoystate)(void * ptr);
  virtual uint8_t __not_in_flash_func(kempston)();
  virtual uint8_t __not_in_flash_func(sinclairL)();
  virtual uint8_t __not_in_flash_func(sinclairR)();
  virtual uint8_t __not_in_flash_func(joy1)();
  virtual uint8_t __not_in_flash_func(joy2)();
  virtual bool __not_in_flash_func(isConnectedL)();
  virtual bool __not_in_flash_func(isConnectedR)();
};
