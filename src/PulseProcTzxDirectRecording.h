#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"
#include "PulseProcBitStream.h"
#include "PulseProcPauseMillis.h"

class PulseProcTzxDirectRecording : public PulseProc {
private:

  PulseProcBitStream* _data;
  PulseProcTone* _end;
  PulseProcPauseMillis* _pause;
  uint32_t _tsPerMs;

public:

  PulseProcTzxDirectRecording(
    PulseProcBitStream* data,
    PulseProcTone* end,
    PulseProcPauseMillis* pause
  );
  
  void init(
    PulseProc *nxt,
    uint32_t tsPerMs
  );
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
