#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcStdByte : public PulseProc {
private:

  PulseProcTone* _t1;
  uint32_t _b;
  uint32_t _ts[2];
  
public:

  PulseProcStdByte(PulseProcTone* t1);
  
  void init(PulseProc *next, uint32_t b);
  
  void __not_in_flash_func(init)(
    PulseProc *nxt,
    uint32_t b,
    uint32_t n,
    uint32_t ts0,
    uint32_t ts1
  );
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
