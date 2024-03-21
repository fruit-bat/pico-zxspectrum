#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcStdHeader.h"
#include "PulseProcStdByteStream.h"
#include "PulseProcPauseMillis.h"

class PulseProcTzxTurbo : public PulseProc {
private:

  PulseProcStdHeader* _header;
  PulseProcStdByteStream* _data;
  PulseProcPauseMillis* _pause;
  uint32_t _tsPerMs;

public:

  PulseProcTzxTurbo(
    PulseProcStdHeader* header,
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
