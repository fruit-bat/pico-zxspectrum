#include "PulseProcTone.h"

int32_t __not_in_flash_func(PulseProcTone::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_n == 0) return PP_COMPLETE;
  _n--;
  *pstate = !*pstate;
  return _l;
}
