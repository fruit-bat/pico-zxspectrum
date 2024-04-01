#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcTzxPureTone : public PulseProc {
private:

  PulseProcTone* _ppTone;

public:

  PulseProcTzxPureTone(
    PulseProcTone* ppTone
  );
  
  void init(
    PulseProc *nxt
  );
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
