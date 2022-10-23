#include "PulseProcStdByteStream.h"

PulseProcStdByteStream::PulseProcStdByteStream(PulseProcStdByte* ppb) :
  _ppb(ppb),
  _l(0)
{}
  
void PulseProcStdByteStream::init(PulseProc *nxt, uint32_t l) {
  next(nxt);
  _l = l;
}

int32_t __not_in_flash_func(PulseProcStdByteStream::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_l == 0) return PP_COMPLETE;
  int32_t r = is->readByte();
  if (r < 0) {
    if (r == -1) {
      *pstate = !*pstate;
    }  
    return PP_ERROR;
  } 
  _ppb->init(this, r);
  *top = _ppb;
  return PP_CONTINUE;
}
