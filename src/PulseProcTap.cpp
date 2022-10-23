#include "PulseProcTap.h"


PulseProcTap::PulseProcTap(
  PulseProcStdHeader* header,
  PulseProcStdByte* byte,
  PulseProcStdByteStream* data
) : 
  _header(header),
  _byte(byte),
  _data(data)
{
}

void PulseProcTap::init(PulseProc *nxt) {
  next(nxt);
}

int32_t __not_in_flash_func(PulseProcTap::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTap: \n");
  const int8_t l[] = {2, 1};
  uint32_t h[2];
  int32_t r = is->decodeLsbf(h, l, 2);
  if (r < 0) {
    if (r == -1) {
      DBG_PULSE("PulseProcTap: ROF reading TAP header\n");
      return PP_COMPLETE;
    }
    else {
      DBG_PULSE("PulseProcTap: Error (%ld) reading TAP header\n", r);
      return PP_ERROR;
    }
  }
  else {
    uint32_t l = h[0];
    uint32_t m = h[1];
    DBG_PULSE("PulseProcTap: Read standard TAP header of length %ld, marker %02lX\n", l, m);
    if (l < 1) {
      DBG_PULSE("PulseProcTap: Error length %ld is too small\n", l);
      return PP_ERROR;
    }
    _header->init(_byte, m ? 3223 : 8063);
    _byte->init(_data, m);
    _data->init(next(), l - 1);
    *top = _header;
    return PP_CONTINUE;
  }
}
