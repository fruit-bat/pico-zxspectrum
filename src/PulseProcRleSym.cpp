#include "PulseProcRleSym.h"

int32_t __not_in_flash_func(PulseProcRleSym::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (!_pi) {
    if (_si >= _symbols->as()) {
      DBG_PULSE("PulseProcRleSym: symbol index out of range %ld\n", _si);
      return PP_ERROR;
    }
    uint16_t v = _symbols->at(_si, _pi++);
    switch(v) {
      case 0: *pstate = !*pstate; break;
      case 1: break;
      case 2: *pstate = false; break;
      case 3: *pstate = true; break;
      default:
        DBG_PULSE("PulseProcRleSym: Unexpected symbol type %d\n", v);
        return PP_ERROR;
    }
  }
  
  if (_pi > _symbols->np()) return PP_COMPLETE;
  
  return _symbols->at(_si, _pi++);
}
