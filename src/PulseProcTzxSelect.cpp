#include "PulseProcTzxSelect.h"
#include <cstring>

PulseProcTzxSelect::PulseProcTzxSelect() 
{}
  
void PulseProcTzxSelect::init(PulseProc *nxt) {
  next(nxt);
}

int32_t __not_in_flash_func(PulseProcTzxSelect::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  
  DBG_PULSE("About to read TZX select\n");

  return PP_COMPLETE;
}
