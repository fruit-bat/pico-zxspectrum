#pragma once

#include "PulseProc.h"
#include <pico/stdlib.h>
#include <pico/printf.h>
#include "InputStream.h"
#include <vector>

class PulseProcSymdefs : public PulseProc {
private:

  std::vector<uint16_t>* _sd;
  uint32_t _np;
  uint32_t _as;

public:

  PulseProcSymdefs(std::vector<uint16_t>* sd);
  
  void init(
    PulseProc *nxt, 
    uint32_t np,
    uint32_t as
  );

  int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
  
  inline uint16_t at(uint32_t si, uint32_t pi) {
    uint32_t i = pi + (__mul_instruction(si, (_np + 1)));
    if (i > _sd->size()) { // TODO in theory this test can be removed
      DBG_PULSE("PulseProcSymdefs: symbol index out of range %ld\n", i);
      return 0;
    }
    return _sd->at(i);
  }
  
  inline uint32_t np() {
    return _np;
  }
  
  inline uint32_t as() {
    return _as;
  }  
};
