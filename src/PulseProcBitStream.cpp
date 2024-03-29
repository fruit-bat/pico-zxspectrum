#include "PulseProcBitStream.h"

PulseProcBitStream::PulseProcBitStream() :
  _l(0)
{}

void PulseProcBitStream::init(
    PulseProc *nxt,
    uint32_t l,
    uint32_t tspb,
    uint32_t blb) {
  next(nxt);
  _l = l;
  _tspb = tspb;
  _blb = blb > 8 ? 8 : blb;
  _b = 0x10000;
}

int32_t __not_in_flash_func(PulseProcBitStream::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_l == 0) return PP_COMPLETE;
  if (_b & 0x10000) {
    int32_t r = is->readByte();
    if (r < 0) return PP_ERROR;
    if (_l > 1 || _blb == 8) {
      if (r == 0) {
        *pstate = false;
        --_l;
        return _tspb << 3;
      }
      else if (r == 0xff) {
        *pstate = true;
        --_l;
        return _tspb << 3;
      }
    }
    _b = r | (1 << (--_l ? 8 : 16 - _blb));
  }
  
  bool s = (((_b >> 7) & 1) == 1);
  
  *pstate = s;
  
  uint32_t d = 0;
  do {
    _b <<= 1;
    d +=_tspb;
  } while (!(_b & 0x10000) && (s == (((_b >> 7) & 1) == 1)));
  
  return d;
}
