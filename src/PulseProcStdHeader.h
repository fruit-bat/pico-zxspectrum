#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcStdHeader : public PulseProc {
private:

  PulseProcTone* _t1;
  uint32_t _step;
  uint32_t _p;
  
public:

  PulseProcStdHeader(PulseProcTone* t1);
  
  void init(PulseProc *next, uint32_t p);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
