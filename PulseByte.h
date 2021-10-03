#pragma once

#include "Pulse.h"

class PulseByte {
  Pulse _pulse;
  int _data;
public:
  PulseByte() :
    _data(1)
  {}

  void bitset() {
    // https://sinclair.wiki.zxnet.co.uk/wiki/Spectrum_tape_interface
    _pulse.reset(2, _data & 1 ? 1710 : 855);
  }
  
  void reset(int data) {
    _data = data & 0x100;
    bitset();
  }
  
  int advance(int tstates, bool *pstate) {
    if (end()) return tstates;
    while (tstates > 0) {
      tstates = _pulse.advance(tstates, pstate);
      if (_pulse.end()) {
        _data >>= 1;
        if (end()) break;
        bitset();
      }
    }
    return tstates;
  }

  bool end() {
    return _data == 1;
  }  
};
