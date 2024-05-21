#pragma once

#include <pico/printf.h>

enum ZxSpectrumJoystickMode {
  ZxSpectrumJoystickModeKempston,
  ZxSpectrumJoystickModeSinclairLR,
  ZxSpectrumJoystickModeSinclairRL
};

class ZxSpectrumJoystick {
private:
  ZxSpectrumJoystickMode _mode;
public:
  ZxSpectrumJoystick() :
    _mode(ZxSpectrumJoystickModeKempston)
  {}

  virtual uint8_t kempston() = 0;
  virtual uint8_t sinclairL() = 0;
  virtual uint8_t sinclairR() = 0;
  virtual bool isConnectedL() = 0;
  virtual bool isConnectedR() = 0;

  virtual void mode(ZxSpectrumJoystickMode mode) { _mode = mode; }

  inline ZxSpectrumJoystickMode mode() { return _mode; }
  uint8_t __not_in_flash_func(getKempston)();
  uint8_t __not_in_flash_func(getSinclairL)();
  uint8_t __not_in_flash_func(getSinclairR)();
};
