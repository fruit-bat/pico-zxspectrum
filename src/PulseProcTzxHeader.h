#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"

class PulseProcTzxHeader : public PulseProc {
private:
 
public:

  PulseProcTzxHeader();
  
  void init(PulseProc *next);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
