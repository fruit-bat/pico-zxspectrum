#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include <vector>
#include "PulseProcTzxHeader.h"
#include "PulseProcTzxGlue.h"
#include "PulseProcTzxIndex.h"

class PulseProcTzx : public PulseProc {
private:
  std::vector<uint32_t> _bi;
  PulseProcTzxHeader _pptHeader;
  PulseProcTzxGlue _pptGlue;
  PulseProcTzxIndex _pptIndex;
  
public:

  PulseProcTzx();
  
  void init(PulseProc *next);
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
