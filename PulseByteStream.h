#pragma once

#include "InputStream.h"
#include "PulseByte.h"
#include <pico/printf.h>

class PulseByteStream {
  InputStream* _is;
  int _n;
  PulseByte _pulseByte;
public:
  PulseByteStream() :
    _is(0),
    _n(0)
  {}

  void reset(InputStream* is, int n) {
    _is = is;
    _n = n == 0 ? 0 : n + 1;
  }

  // Can return -ve for error
  int advance(int *tstates, bool *pstate) {
    if (end()) return 0;
    while(*tstates > 0) {
      if (_pulseByte.end()) {
        --_n;
        if (end()) break;
        int b = _is->readByte();
        printf("Read tap data %d\n", b);
        if (b < 0) {
          _n = 0;
          _is = 0;
          return b;
        }
        _pulseByte.reset(b);
      }
      _pulseByte.advance(tstates, pstate);
    }
    return 0;
  }

  bool end() {
    return (_n == 0) || (_is == 0);
  }
};
