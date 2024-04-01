#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcStdHeader : public PulseProc {
private:

  PulseProcTone* _t1;
  uint32_t _step;
  uint32_t _p;
  uint32_t _hts;
  uint32_t _s1ts;
  uint32_t _s2ts;
  
public:

  PulseProcStdHeader(PulseProcTone* t1);
  
  void init(PulseProc *next, uint32_t p);
  
  void init(
    PulseProc *next,
    uint32_t p,
    uint32_t hts,
    uint32_t s1ts,
    uint32_t s2ts
  );
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
