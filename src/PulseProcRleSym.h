#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcSymbol.h"

class PulseProcRleSym : public PulseProc {
private:
  PulseProcSymbol* _symbol;
  uint32_t _ns;
  uint32_t _nr;
  uint32_t _si;

public:

  PulseProcRleSym(PulseProcSymbol* symbol) :
  _symbol(symbol),
  _ns(0)
  {}
  
  inline void init(
    PulseProc *nxt, 
    uint32_t ns)
  {
    next(nxt);
    _ns = ns;
    _nr = 0;
  }
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
