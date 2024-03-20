#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcPulseStream : public PulseProc {
private:

  PulseProcTone* _ppTone;
  uint32_t _l;
  
public:

  PulseProcPulseStream(PulseProcTone* ppTone);
  
  void init(PulseProc *next, uint32_t l);
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
