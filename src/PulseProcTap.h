#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include "PulseProcTone.h"
#include "PulseProcStdHeader.h"
#include "PulseProcStdByteStream.h"
#include "PulseProcStdByte.h"

class PulseProcTap : public PulseProc {
private:

  PulseProcStdHeader* _header;
  PulseProcStdByte* _byte;
  PulseProcStdByteStream* _data;
  PulseProcTone* _end;
  
public:

  PulseProcTap(
    PulseProcStdHeader* header,
    PulseProcStdByte* byte,
    PulseProcStdByteStream* data,
    PulseProcTone* end
  );
  
  void init(PulseProc *next);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
