#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcSymbols.h"

class PulseProcRleSym : public PulseProc {
private:
  PulseProcSymbols* _symbols;
  uint32_t _si; // The symbol index
  uint32_t _pi; // The pulse index

public:

  PulseProcRleSym() :
  _symbols(0),
  _si(0),
  _pi(0)
  {}
  
  inline void init(
    PulseProc *nxt, 
    PulseProcSymbols* symbols,
    uint32_t si)
  {
    next(nxt);
    _symbols = symbols;
    _si = si;
    _pi = 0;
  }
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
