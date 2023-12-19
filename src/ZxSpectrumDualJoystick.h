#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"

class ZxSpectrumDualJoystick : public ZxSpectrumJoystick {
private:
  ZxSpectrumJoystick *_joystick1;
  ZxSpectrumJoystick *_joystick2;
public:
  ZxSpectrumDualJoystick(
    ZxSpectrumJoystick *joystick1,
    ZxSpectrumJoystick *joystick2);
    
  virtual void mode(ZxSpectrumJoystickMode mode);
  virtual uint8_t __not_in_flash_func(kempston)();
  virtual uint8_t __not_in_flash_func(sinclairL)();
  virtual uint8_t __not_in_flash_func(sinclairR)();
  virtual bool __not_in_flash_func(isConnectedL)() { return _joystick1->isConnectedL(); };
  virtual bool __not_in_flash_func(isConnectedR)() { return _joystick1->isConnectedR(); };
};
