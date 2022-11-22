#include "PulseProcTzx.h"
#include <cstring>

PulseProcTzx::PulseProcTzx(
  PulseProcTap* ppTap,
  PulseProcStdHeader* header,
  PulseProcStdByteStream* data,
  PulseProcTone* ppTone1,
  PulseProcPauseMillis* pause
) :
  _pptBlock(ppTap, header, data, ppTone1, pause),
  _tsPerMs(0)
{}
  
void PulseProcTzx::init(
    PulseProc *nxt,
    uint32_t tsPerMs,
    bool is48k
) {
  next(nxt);
  _tsPerMs = tsPerMs;
  _is48k = is48k;
}

int32_t PulseProcTzx::advance(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzx: Loading TZX\n");

  _pptHeader.init(&_pptIndex);
  _pptIndex.init(&_pptBlock, &_bi);
  _pptBlock.init(&_lastPause, &_bi, _tsPerMs, _is48k);
  _lastPause.init(next(), 1000, _tsPerMs);
  *top = &_pptHeader;
  
  return PP_CONTINUE;
}
