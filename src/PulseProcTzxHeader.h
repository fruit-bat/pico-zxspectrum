#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"

class PulseProcTzxHeader : public PulseProc {
private:
 
public:

  PulseProcTzxHeader();
  
  void init(PulseProc *next);
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
