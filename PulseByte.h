#pragma once

#include "Pulse.h"

class PulseByte {
  Pulse _pulse;
  int _data;

  void bitset() {
    _pulse.reset(2, _data & 0x100 ? 1710 : 855);
  }
  
public:
  PulseByte() :
    _data(0x100)
  {}

  void reset(int data) {
    _data = (data << 1) | 1;
    bitset();
  }

  void clear() {
    _data = 0x100;
  }
  
  void advance(int *tstates, bool *pstate) {
    if (end()) return;
    while (*tstates > 0) {
      _pulse.advance(tstates, pstate);
      if (_pulse.end()) {
        _data <<= 1;
        if (end()) break;
        bitset();
      }
    }
  }

  bool end() {
    return (_data & 0x1ff) == 0x100;
  }  
};
