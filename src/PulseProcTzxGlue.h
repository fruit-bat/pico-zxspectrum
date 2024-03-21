#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"

class PulseProcTzxGlue : public PulseProc {
private:
 
public:

  PulseProcTzxGlue();
  
  void init(PulseProc *next);
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
