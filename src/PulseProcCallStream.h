#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcCallStream : public PulseProc {
private:
  uint32_t _l;
  uint32_t* _i;
  uint32_t _in;
  uint32_t _p;
  
public:

  PulseProcCallStream(uint32_t* i);
  
  void init(PulseProc *next, uint32_t l, uint32_t p);
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
