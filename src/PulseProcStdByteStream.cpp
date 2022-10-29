#include "PulseProcStdByteStream.h"

PulseProcStdByteStream::PulseProcStdByteStream(PulseProcStdByte* ppb) :
  _ppb(ppb),
  _l(0)
{}

void PulseProcStdByteStream::init(PulseProc *nxt, uint32_t l) {
  next(nxt);
  _l = l;
  _ts0 = 855;
  _ts1 = 1710;
  _blb = 8;
}

void PulseProcStdByteStream::init(
  PulseProc *nxt,
  uint32_t l,
  uint32_t ts0,
  uint32_t ts1,
  uint32_t blb  
) {
  next(nxt);
  _l = l;
  _ts0 = ts0;
  _ts1 = ts1;
  _blb = blb;
}

int32_t __not_in_flash_func(PulseProcStdByteStream::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_l == 0) return PP_COMPLETE;
  int32_t r = is->readByte();
  if (r < 0) return PP_ERROR;
  _ppb->init(
    this,
    r,
    _l == 1 ? _blb : 8,
    _ts0,
    _ts1
  );
  --_l;
  *top = _ppb;
  return PP_CONTINUE;
}
