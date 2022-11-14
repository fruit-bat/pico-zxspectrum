#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"

class PulseProcTzxSelect : public PulseProc {
private:
 
public:

  PulseProcTzxSelect();
  
  void init(PulseProc *next);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
