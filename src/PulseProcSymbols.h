#pragma once

#include "PulseProc.h"
#include <pico/stdlib.h>
#include <pico/printf.h>
#include "InputStream.h"
#include <vector>

class PulseProcSymbols {
  
  uint32_t _np;
  uint32_t _as;
  std::vector<uint16_t> _sd;

public:

  int32_t read(
    InputStream* is,
    uint32_t np,
    uint32_t as
  );

};
