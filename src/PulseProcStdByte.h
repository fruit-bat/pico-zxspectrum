#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcStdByte : public PulseProc {
private:

  PulseProcTone* _t1;
  uint32_t _b;
  
public:

  PulseProcStdByte(PulseProcTone* t1);
  
  void init(PulseProc *next, uint32_t b);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
