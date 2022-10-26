#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"

class PulseProcPauseMillis : public PulseProc {
private:

  uint32_t _ms;      // milliseconds to pause
  uint32_t _tsPerMs; // t-states per millisecond
  
public:

  PulseProcPauseMillis() :
  _ms(0),
  _tsPerMs(3555)
  {}
  
  inline void init(
    PulseProc *nxt, 
    uint32_t ms,
    uint32_t tsPerMs)
  {
    next(nxt);
    _ms = ms;
    _tsPerMs = tsPerMs;
  }
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
