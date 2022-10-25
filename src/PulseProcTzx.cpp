#include "PulseProcTzx.h"
#include <cstring>

PulseProcTzx::PulseProcTzx()
{}
  
void PulseProcTzx::init(PulseProc *nxt) {
  next(nxt);
}

int32_t PulseProcTzx::advance(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzx: Loading TZX\n");

  _pptHeader.init(&_pptIndex);
  _pptIndex.init(&_pptBlock, &_bi);
  _pptBlock.init(next(), &_bi);
  *top = &_pptHeader;
  
  return PP_CONTINUE;
}
