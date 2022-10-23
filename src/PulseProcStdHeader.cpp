#include "PulseProcStdHeader.h"

PulseProcStdHeader::PulseProcStdHeader(PulseProcTone* t1) :
  _t1(t1),
  _step(0),
  _p(0)
{}
  
void PulseProcStdHeader::init(PulseProc *nxt, uint32_t p) {
  next(nxt);
  _step = 3;
  _p = p;
}

int32_t __not_in_flash_func(PulseProcStdHeader::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcStdHeader: Step %ld\n", _step);
  if (_step == 0) return PP_COMPLETE;
  switch(_step--) {
    case 3: _t1->init(this, 2168, _p); break;
    case 2: _t1->init(this, 667,   1); break;
    case 1: _t1->init(this, 735,   1); break;
  }
  *top = _t1;
  return PP_CONTINUE;
}
