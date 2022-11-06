#include "PulseProcSymbol.h"

int32_t __not_in_flash_func(PulseProcSymbol::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (!_pi) {
    if (_si >= _symdefs->as()) {
      DBG_PULSE("PulseProcSymbol: symbol index out of range %ld\n", _si);
      return PP_ERROR;
    }
    uint16_t v = _symdefs->at(_si, _pi++);
    switch(v) {
      case 0: break;
      case 1: *pstate = !*pstate; break;
      case 2: *pstate = false; break;
      case 3: *pstate = true; break;
      default:
        DBG_PULSE("PulseProcSymbol: Unexpected symbol type %d\n", v);
        return PP_ERROR;
    }
    
    if (_pi > _symdefs->np()) return PP_COMPLETE;
    uint16_t p = _symdefs->at(_si, _pi++);
    if (!p) return PP_COMPLETE;
    return p;
  }
  else {
    *pstate = !*pstate;
    if (_pi > _symdefs->np()) return PP_COMPLETE;
    uint16_t p = _symdefs->at(_si, _pi++);
    if (!p) return PP_COMPLETE;
    return p;
  }
}
