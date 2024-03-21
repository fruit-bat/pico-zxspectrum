#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcSymbol.h"

class PulseProcSymbolData : public PulseProc {
private:
  PulseProcSymbol* _symbol;
  uint32_t _ns;
  uint32_t _bps;
  uint32_t _sd;
  uint32_t _sb;
  uint32_t _sm;

public:

  PulseProcSymbolData(PulseProcSymbol* symbol) :
  _symbol(symbol),
  _ns(0),
  _bps(1),
  _sb(0)
  {}
  
  inline void init(
    PulseProc *nxt, 
    uint32_t ns,
    uint32_t bps)
  {
    next(nxt);
    _ns = ns;
    _bps = bps;
    _sm = (1 << bps) - 1;
    _sd = 0;
    _sb = 0;
  }
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
