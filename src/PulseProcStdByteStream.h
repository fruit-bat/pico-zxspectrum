#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcStdByte.h"

class PulseProcStdByteStream : public PulseProc {
private:

  PulseProcStdByte* _ppb;
  uint32_t _l;
  uint32_t _ts0;
  uint32_t _ts1;
  uint32_t _blb;
  
public:

  PulseProcStdByteStream(PulseProcStdByte* ppb);
  
  void init(PulseProc *next, uint32_t l);
  
  void init(
    PulseProc *nxt,
    uint32_t l,
    uint32_t ts0,
    uint32_t ts1,
    uint32_t blb  
  );
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
