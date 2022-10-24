#include "PulseProcTzxHeader.h"
#include <cstring>

PulseProcTzxHeader::PulseProcTzxHeader() 
{}
  
void PulseProcTzxHeader::init(PulseProc *nxt) {
  next(nxt);
}

int32_t PulseProcTzxHeader::advance(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  uint8_t header[10];
  
  DBG_PULSE("PulseProcTzxHeader: About to read TZX header\n");
  int l = is->read(header, 10);

  if (l < 10) {
    DBG_PULSE("PulseProcTzxHeader: Could not read complete TZX header\n");
    return PP_ERROR;
  }
  
  if (strncmp((const char*)header, "ZXTape!", 7) != 0) {
    DBG_PULSE("PulseProcTzxHeader: Could not verify TZX signature bytes\n");
    return PP_ERROR;
  }
  
  if (header[7] != 0x1a) {
    DBG_PULSE("PulseProcTzxHeader: Could not find end of text file marker\n");
    return  PP_ERROR;
  }
  
  uint8_t version_major = header[8];
  uint8_t version_minor = header[9];
  
  DBG_PULSE("PulseProcTzxHeader: Found TZX header version %d %d\n", version_major, version_minor);

  return PP_COMPLETE;
}
