#include "PulseProcTzxGlue.h"
#include <cstring>

PulseProcTzxGlue::PulseProcTzxGlue() 
{}
  
void PulseProcTzxGlue::init(PulseProc *nxt) {
  next(nxt);
}

int32_t __not_in_flash_func(PulseProcTzxGlue::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  uint8_t header[9];
  
  DBG_PULSE("About to read TZX glue\n");
  int l = is->read(header, 9);

  if (l < 9) {
    DBG_PULSE("Could not read complete TZX glue\n");
    return PP_ERROR;
  }
  
  if (strncmp((const char*)header, "XTape!", 6) != 0) {
    DBG_PULSE("Could not verify TZX glue bytes\n");
    return PP_ERROR;
  }
  
  if (header[6] != 0x1a) {
    DBG_PULSE("Could not find end of text file marker\n");
    return  PP_ERROR;
  }
  #ifdef DEBUG_PULSE
  uint8_t version_major = header[7];
  uint8_t version_minor = header[8];
  
  DBG_PULSE("Found TZX glue version %d %d\n", version_major, version_minor);
  #endif
  return PP_COMPLETE;
}
