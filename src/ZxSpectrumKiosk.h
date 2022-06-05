#pragma once

#include <pico/printf.h>

class ZxSpectrumKiosk {
public:
  virtual ~ZxSpectrumKiosk() {}
  virtual bool isKiosk() { return false; }
};
