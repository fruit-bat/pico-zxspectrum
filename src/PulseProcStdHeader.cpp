#include "PulseProcStdHeader.h"

PulseProcStdHeader::PulseProcStdHeader(PulseProcTone* t1) :
  _t1(t1),
  _step(0),
  _p(0)
{}
  
void PulseProcStdHeader::init(uint32_t p) {
  _step = 3;
  _p = p;
  _t1->next(this);
}

int32_t __not_in_flash_func(PulseProcStdHeader::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_step == 0) return PP_COMPLETE;
  switch(_step--) {
    case 3: _t1->init(2168, _p); break;
    case 2: _t1->init(667,   1); break;
    case 1: _t1->init(735,   1); break;
  }
  *top = _t1;
  return PP_CONTINUE;
}
