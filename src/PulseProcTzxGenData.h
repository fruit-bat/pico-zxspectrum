#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcPauseMillis.h"
#include "PulseProcSymbols.h"

class PulseProcTzxGenData : public PulseProc {
private:

  PulseProcPauseMillis* _pause;
  PulseProcSymbols _symbols;
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
