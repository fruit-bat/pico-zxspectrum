#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcStdByte.h"

class PulseProcStdByteStream : public PulseProc {
private:

  PulseProcStdByte* _ppb;
  uint32_t _l;
  
public:

  PulseProcStdByteStream(PulseProcStdByte* ppb);
  
  void init(PulseProc *next, uint32_t l);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
