#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"
#include "PulseProcStdHeader.h"
#include "PulseProcStdByteStream.h"
#include "PulseProcStdByte.h"
#include "PulseProcPauseMillis.h"

class PulseProcTzxPureTone : public PulseProc {
private:

  PulseProcTone* _ppTone;

public:

  PulseProcTzxPureTone(
    PulseProcTone* ppTone
  );
  
  void init(
    PulseProc *nxt
  );
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
