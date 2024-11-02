#pragma once

#include "ZxSpectrumPort.h"

class WavesharePiZeroPort : public ZxSpectrumPort {
public:
  uint8_t read();
  void write(uint8_t data);
  void init();
};
