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

  void __not_in_flash_func(reset)(InputStream* is, int n) {
    _is = is;
    _n = n == 0 ? 0 : n + 1;
    _pulseByte.clear();
  }

  // Can return -ve for error
  int __not_in_flash_func(advance)(int *tstates, bool *pstate) {
    if (end()) return 0;
    while(*tstates > 0) {
      if (_pulseByte.end()) {
        --_n;
        if (end()) break;
        int b = _is->readByte();
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

  bool __not_in_flash_func(end)() {
    return (_n == 0) || (_is == 0);
  }
};
