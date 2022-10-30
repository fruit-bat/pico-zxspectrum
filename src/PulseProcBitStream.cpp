#include "PulseProcBitStream.h"

PulseProcBitStream::PulseProcBitStream(PulseProcTone* ppTone) :
  _ppTone(ppTone),
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
    _b = r | (1 << (_l == 1 ? (16 - _blb) : 8));
  }
  *pstate = (_b >> 7) & 1;
  *top = _ppTone;
  _b <<= 1;
  return _tspb;
}
