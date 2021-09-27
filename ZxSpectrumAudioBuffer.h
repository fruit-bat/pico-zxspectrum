#pragma once
#include <pico/printf.h>

#define LEN_BITS 6
#define LEN (1 << LEN_BITS)
#define LEN_MASK (LEN-1)

class ZxSpectrumAudioBuffer {
  bool _state;
  bool _lastState;
  int _buffer[LEN];
  unsigned int _w;
  unsigned int _r;
    
public:

  void set(bool state) {
    _state = state;
  }
  /*
  void tick(unsigned int tstates) {
    int c = _buffer[_w];
    
    
    
    if (((c > 0) && !_state) || ((c < 0) && _state)) {
      // move forwards
      _w = (_w + 1) & LEN_MASK;
      if (_w == _r) {
        printf("Audio overrun\n");
      }
      
    }
  }
  */
};
