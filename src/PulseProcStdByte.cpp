#include "PulseProcStdByte.h"

PulseProcStdByte::PulseProcStdByte(PulseProcTone* t1) :
  _t1(t1),
  _b(0x10000)
{}
  
void PulseProcStdByte::init(PulseProc *nxt, uint32_t b) {
  next(nxt);
  _b = b | 0x100;
}

int32_t __not_in_flash_func(PulseProcStdByte::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_b & 0x10000) return PP_COMPLETE;
  _t1->init(this, _b & 0x80 ? 1710 : 855, 2);
  _b <<= 1;
  *top = _t1;
  return PP_CONTINUE;
}
