#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include <vector>

class PulseProcTzxSelect : public PulseProc {
private:
  std::vector<uint16_t> _offsets;
public:

  PulseProcTzxSelect();
  
  void init(PulseProc *next);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
  
  int32_t offset(uint32_t choice);
};
