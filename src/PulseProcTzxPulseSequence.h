#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcPulseStream.h"


class PulseProcTzxPulseSequence : public PulseProc {
private:

  PulseProcPulseStream* _ppPulseStream;

public:

  PulseProcTzxPulseSequence(
    PulseProcPulseStream* ppPulseStream
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
