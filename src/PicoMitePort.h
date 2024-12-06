#pragma once

#include "ZxSpectrumPort.h"

class PicoMitePort : public ZxSpectrumPort {
public:
  uint8_t read();
  void write(uint8_t data);
  void init();
};
