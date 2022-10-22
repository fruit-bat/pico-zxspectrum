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
private:

  PulseProc *_next;
  
public:

  PulseProc();

  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
  
  PulseProc *next() { return _next; }
  void next(PulseProc *pp) { _next = pp; }
};
