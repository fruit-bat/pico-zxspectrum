#include "PulseProcCallStream.h"

PulseProcCallStream::PulseProcCallStream(uint32_t* i) :
  _l(0),
  _i(i),
  _p(0)
{}

void PulseProcCallStream::init(PulseProc *nxt, uint32_t l, uint32_t p) {
  next(nxt);
  _l = l;
  _in = *_i;
  _p = p;
}

int32_t __not_in_flash_func(PulseProcCallStream::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_l == 0) {
    *_i  = _in;
    return PP_COMPLETE;
  }
  int32_t r;
  r = is->seek(_p);
  if (r < 0) return PP_ERROR;
  r = is->readWord();
  if (r < 0) return PP_ERROR;
  _p = is->pos();
  *_i = r;
  --_l;
  return PP_COMPLETE;
}
