#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcSymdefs.h"

class PulseProcSymbol : public PulseProc {
private:
  PulseProcSymdefs* _symdefs;
  uint32_t _si; // The symbol index
  uint32_t _pi; // The pulse index

public:

  PulseProcSymbol(PulseProcSymdefs* symdefs) :
  _symdefs(symdefs),
  _si(0),
  _pi(0)
  {}
  
  inline void init(
    PulseProc *nxt,
    uint32_t si)
  {
    next(nxt);
    _si = si;
    _pi = 0;
  }

  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
