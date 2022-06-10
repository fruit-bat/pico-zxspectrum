#pragma once

#include <pico/printf.h>

class ZxSpectrumJoystick {
  
public:
  virtual uint8_t kempston();
  virtual uint8_t sinclairL();
  virtual uint8_t sinclairR();
  virtual bool isConnectedL();
  virtual bool isConnectedR();
};
