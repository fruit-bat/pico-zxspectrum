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
  _hts = 2168;
  _s1ts = 667;
  _s2ts = 735;
}

void PulseProcStdHeader::init(
  PulseProc *nxt,
  uint32_t p,
  uint32_t hts,
  uint32_t s1ts,
  uint32_t s2ts
) {
  next(nxt);
  _step = 3;
  _p = p;
  _hts = hts;
  _s1ts = s1ts;
  _s2ts = s2ts;
}

int32_t __not_in_flash_func(PulseProcStdHeader::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcStdHeader: Step %ld\n", _step);
  if (_step == 0) return PP_COMPLETE;
  switch(_step--) {
    case 3: _t1->init(this, _hts,  _p); break;
    case 2: _t1->init(this, _s1ts,  1); break;
    case 1: _t1->init(this, _s2ts,  1); break;
  }
  *top = _t1;
  return PP_CONTINUE;
}
