#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcStdByteStream.h"
#include "PulseProcPauseMillis.h"

class PulseProcTzxPureData : public PulseProc {
private:

  PulseProcStdByteStream* _data;
  PulseProcPauseMillis* _pause;
  uint32_t _tsPerMs;

public:

  PulseProcTzxPureData(
    PulseProcStdByteStream* data,
    PulseProcPauseMillis* pause
  );
  
  void init(
    PulseProc *nxt,
    uint32_t tsPerMs
  );
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
