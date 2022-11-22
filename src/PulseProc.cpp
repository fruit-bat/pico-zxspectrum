#include "PulseProc.h"

PulseProc::PulseProc() : _next(0) 
{
}

int32_t __not_in_flash_func(PulseProc::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top) 
{
  return PP_COMPLETE;
}
