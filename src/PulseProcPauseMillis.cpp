#include "PulseProcPauseMillis.h"

int32_t __not_in_flash_func(PulseProcPauseMillis::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_ms == 0) return PP_COMPLETE;
  --_ms;
  return _tsPerMs;
}
