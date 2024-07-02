#pragma once
#include <pico/stdlib.h>


class ZxSpectrumMouse {
private:
public:
  ZxSpectrumMouse() {}
  virtual uint8_t buttons() = 0;
  virtual uint8_t xAxis() = 0;
  virtual uint8_t yAxis() = 0;
  virtual bool isMounted() = 0;
};
