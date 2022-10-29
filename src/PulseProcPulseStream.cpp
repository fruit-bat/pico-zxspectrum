#include "PulseProcPulseStream.h"

PulseProcPulseStream::PulseProcPulseStream(PulseProcTone* ppTone) :
  _ppTone(ppTone),
  _l(0)
{}

void PulseProcPulseStream::init(PulseProc *nxt, uint32_t l) {
  next(nxt);
  _l = l;
}

int32_t __not_in_flash_func(PulseProcPulseStream::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_l == 0) return PP_COMPLETE;
  int32_t r = is->readWord();
  if (r < 0) return PP_ERROR;
  _ppTone->init(
    this,
    r,
    1
  );
  --_l;
  *top = _ppTone;
  return PP_CONTINUE;
}
