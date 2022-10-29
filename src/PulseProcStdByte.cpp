#include "PulseProcStdByte.h"

PulseProcStdByte::PulseProcStdByte(PulseProcTone* t1) :
  _t1(t1),
  _b(0x10000)
{}
  
void PulseProcStdByte::init(PulseProc *nxt, uint32_t b) {
  next(nxt);
  _b = b | 0x100;
  _ts[0] = 855;
  _ts[1] = 1710;
}

void __not_in_flash_func(PulseProcStdByte::init)(
  PulseProc *nxt,
  uint32_t b,
  uint32_t n,
  uint32_t ts0,
  uint32_t ts1
) {
  next(nxt);
  _b = b | (1 << (16 - (n > 8 ? 8 : n)));
  _ts[0] = ts0;
  _ts[1] = ts1;  
}

int32_t __not_in_flash_func(PulseProcStdByte::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_b & 0x10000) return PP_COMPLETE;
  _t1->init(this, _ts[(_b >> 7) & 1], 2);
  _b <<= 1;
  *top = _t1;
  return PP_CONTINUE;
}
