#pragma once
#include <pico/stdlib.h>
#include "ZxSpectrumJoystick.h"

enum ZxSpectrumMouseMode {
  ZxSpectrumMouseModeKempstonMouse,
  ZxSpectrumMouseModeJoystick
};

class ZxSpectrumMouse : public ZxSpectrumJoystick {
private:
  ZxSpectrumMouseMode _mouseMode;
public:
  ZxSpectrumMouse() :
    _mouseMode(ZxSpectrumMouseModeKempstonMouse)
  {}

  inline ZxSpectrumMouseMode mouseMode() { return _mouseMode; }

  virtual void mouseMode(ZxSpectrumMouseMode mode) { _mouseMode = mode; }
  virtual uint8_t buttons() = 0;
  virtual uint8_t xAxis() = 0;
  virtual uint8_t yAxis() = 0;
  virtual bool isMounted() = 0;
};
