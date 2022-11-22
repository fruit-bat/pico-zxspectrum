#include "PulseProcTone.h"

int32_t __not_in_flash_func(PulseProcTone::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_i++) {
    *pstate = !*pstate;
    if (_i > _n) return PP_COMPLETE;
  }
  return _l;
}
