#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"

class PulseProcTone : public PulseProc {
private:

  uint32_t _l; // length of pulse in t-states
  uint32_t _n; // number of pulses
  
public:

  PulseProcTone() :
  _l(0),
  _n(0)
  {}
  
  inline void init(
    PulseProc *nxt, 
    uint32_t l,
    uint32_t n)
  {
    next(nxt);
    _l = l;
    _n = n;
  }
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
