#pragma once

#include <pico/stdlib.h>
#include <pico/printf.h>
#include "InputStream.h"

//#define DEBUG_PULSE
#ifdef DEBUG_PULSE
#define DBG_PULSE(...) printf(__VA_ARGS__)
#else
#define DBG_PULSE(...)
#endif

enum PulseProcState {
  PP_CONTINUE = 0,
  PP_COMPLETE = -1,
  PP_ERROR = -2,
  PP_PAUSE = -4,
  PP_EOF = -5
};

class PulseProc {
private:

  PulseProc *_next;
  
public:

  PulseProc();

  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
  
  PulseProc *next() { return _next; }
  void next(PulseProc *pp) { _next = pp; }
};
