#pragma once

#include <pico/printf.h>

enum ZxSpectrumJoystickMode {
  ZxSpectrumJoystickModeKempston,
  ZxSpectrumJoystickModeSinclair  
};

class ZxSpectrumJoystick {
private:
  ZxSpectrumJoystickMode _mode;
public:
  ZxSpectrumJoystick() : 
    _mode(ZxSpectrumJoystickModeKempston)
  {}

  virtual uint8_t kempston();
  virtual uint8_t sinclairL();
  virtual uint8_t sinclairR();
  virtual bool isConnectedL();
  virtual bool isConnectedR();
  
  void mode(ZxSpectrumJoystickMode mode) { _mode = mode; }
  ZxSpectrumJoystickMode mode() { return _mode; }
  inline uint8_t getKempston() { return _mode == ZxSpectrumJoystickModeKempston ? kempston() : 0; }
  inline uint8_t getSinclairL() { return _mode == ZxSpectrumJoystickModeSinclair ? sinclairL() : 0xff; }
  inline uint8_t getSinclairR()  { return _mode == ZxSpectrumJoystickModeSinclair ? sinclairR() : 0xff; } 
};
