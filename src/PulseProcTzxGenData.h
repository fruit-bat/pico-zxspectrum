#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcPauseMillis.h"

class PulseProcTzxGenData : public PulseProc {
private:

  PulseProcPauseMillis* _pause;
  uint32_t _tsPerMs;

public:

  PulseProcTzxGenData(
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
