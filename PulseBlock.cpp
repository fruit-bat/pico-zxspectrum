#include "PulseBlock.h"
#include <pico/printf.h>

void PulseBlock::reset(InputStream* is) {
  int length = is->readWord();
  printf("Read tap block length %d\n", length);
  int marker = is->readByte();
  printf("Read tap marker %d\n", marker);
  if ((length <= 0) || (marker < 0)) {
    is->close();
    return;
  }
  _pulsePreamble.reset(marker);
  _pulseByteStream.reset(is, length - 1); // Take off one for the marker
  _is = is;
  _r = 0;
}

int PulseBlock::advance(int *tstates, bool *pstate) {
  if (end()) return -1;
  _pulsePreamble.advance(tstates, pstate);
  if (!_pulsePreamble.end()) return 0;
  _r = _pulseByteStream.advance(tstates, pstate);
    if (_r == 0 && _pulseByteStream.end()) {
    printf("End of tap block\n");
    reset(_is);
  }
  else if (_r < 0 || _pulseByteStream.end()) {
     printf("End reading tap %d\n", _r);
    _is->close();
    _is = 0;
  }
  return _r;
}

bool PulseBlock::end() {
  return (_is == 0) || (_r < 0) || _pulseByteStream.end();
}
