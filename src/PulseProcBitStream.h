#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"

class PulseProcBitStream : public PulseProc {
private:

  PulseProcTone* _ppTone;
  uint32_t _l;
  uint32_t _tspb;
  uint32_t _blb;
  uint32_t _b;

public:

  PulseProcBitStream(PulseProcTone* ppTone);
  
  void init(
    PulseProc *next,
    uint32_t l,
    uint32_t tspb,
    uint32_t blb
  );
       
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
