#pragma once

#include <pico/stdlib.h>
#include "InputStream.h"

enum PulseProcState {
  PP_COMPLETE = -1,
  PP_ERROR = -2,
  PP_CONTINUE = -3,
  PP_PAUSE = -4
};

class PulseProc {
  PulseProc *_next;
  
public:

  PulseProc(PulseProc *next) : _next(next) {
    
  }

  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
  
  PulseProc *next() { return _next; }
};
