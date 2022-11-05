#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcPauseMillis.h"
#include "PulseProcSymdefs.h"
#include "PulseProcRleSym.h"
#include "PulseProcSymbol.h"
#include <vector>

class PulseProcTzxGenData : public PulseProc {
private:

  std::vector<uint16_t> _sd;
  PulseProcPauseMillis* _pause;
  PulseProcSymdefs _symdefsPilot;
  PulseProcSymbol _symbolPilot;
  PulseProcRleSym _ppRle;
  PulseProcSymdefs _symdefsData;
  PulseProcSymbol _symbolData;
  uint32_t _tsPerMs;

public:

  PulseProcTzxGenData(
    PulseProcPauseMillis* pause
  );
  
  void init(
    PulseProc *nxt,
    uint32_t tsPerMs
  );
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
