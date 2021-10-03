#pragma once

#include "InputStream.h"
#include "PulseByte.h"

class PulseByteStream {
  InputStream _is;
  int _n;
  PulseByte _pulseByte;
public:
  PulseByteStream(InputStream is, int n) :
    _is(is),
    _n(n == 0 ? 0 : n + 1)
  {}

  // Can return -ve for error
  int advance(int tstates, bool *pstate) {
    if (end()) return tstates;
    while(tstates > 0) {
      if (_pulseByte.end()) {
        --_n;
        if (end()) break;
        int b = _is.readByte();
        if (b < 0) return b;
        _pulseByte.reset(b);
      }
      tstates = _pulseByte.advance(tstates, pstate);
    }
    return tstates;
  }

  bool end() {
    return _n == 0;
  }
};
