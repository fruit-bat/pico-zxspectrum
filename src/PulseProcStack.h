#pragma once

#include "PulseProc.h"
#include "InputStream.h"

class PulseProcStack {
private:
  InputStream *_is;
  PulseProc* _top;
  int32_t _state;
  uint32_t _acc;
  
public:

  PulseProcStack();
  void init(InputStream *is, PulseProc* top);
  void __not_in_flash_func(advance)(uint32_t tstates, bool *pstate);
  
  inline bool end() { return _state < 0; }
};
