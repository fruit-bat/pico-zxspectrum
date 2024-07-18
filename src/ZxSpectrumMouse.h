#pragma once
#include <pico/stdlib.h>


enum ZxSpectrumMouseMode {
  ZxSpectrumMouseModeKempstonMouse,
  ZxSpectrumMouseModeJoystick
};

class ZxSpectrumMouse {
private:
  ZxSpectrumMouseMode _mode;
public:
  ZxSpectrumMouse() :
    _mode(ZxSpectrumMouseModeKempstonMouse)
  {}

  inline void mode(ZxSpectrumMouseMode mode) { _mode = mode; }
  inline ZxSpectrumMouseMode mode() { return _mode; }

  virtual uint8_t buttons() = 0;
  virtual uint8_t xAxis() = 0;
  virtual uint8_t yAxis() = 0;
  virtual bool isMounted() = 0;
};
