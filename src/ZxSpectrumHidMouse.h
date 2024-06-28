#pragma once
#include <pico/stdlib.h>
#include "ZxSpectrumMouse.h"

class ZxSpectrumHidMouse : public ZxSpectrumMouse {
private:
  uint32_t _xAcc;
  uint32_t _yAcc;
  uint8_t _buttons;
  uint8_t _mounted;
public:
  ZxSpectrumHidMouse() : _mounted(0) {}

  uint8_t buttons();
  uint8_t xAxis();
  uint8_t yAxis();

  bool isMounted() { return _mounted > 0; }
  inline void mount() { _mounted++; }
  inline void unmount() { _mounted--; }
  inline void xDelta(int32_t xd) { _xAcc += xd; }
  inline void yDelta(int32_t yd) { _yAcc -= yd; }
  void setButtons(uint32_t b);
};
