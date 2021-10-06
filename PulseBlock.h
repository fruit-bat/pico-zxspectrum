#pragma once

#include "PulseByte.h"
#include "PulseByteStream.h"
#include "PulsePreamble.h"

class PulseBlock {
  InputStream* _is;
  PulseByteStream _pulseByteStream;
  PulsePreamble _pulsePreamble;
  int _r;
public:
  PulseBlock() {}
  void reset(InputStream* is);
  int advance(int *tstates, bool *pstate);
  bool end();
};
