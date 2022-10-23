#include "PulseProcChain.h"

PulseProcChain::PulseProcChain() :
  _is(0),
  _top(0),
  _state(PP_COMPLETE),
  _acc(0),
  _ppTone(),
  _ppStdByte(&_ppTone),
  _ppStdByteStream(&_ppStdByte),
  _ppStdHeader(&_ppTone),
  _ppTap(&_ppStdHeader, &_ppStdByte, &_ppStdByteStream)
{
}

void PulseProcChain::init(InputStream *is, PulseProc* top) {
  _is = is;
  _top = top;
  _state = PP_CONTINUE;
  _acc = 0;
}

void __not_in_flash_func(PulseProcChain::advance)(uint32_t tstates, bool *pstate) {
  _acc += tstates;
  while (_state >= 0 && _acc >= static_cast<uint32_t>(_state)) {
    _acc -= _state;
    _state = _top->advance(_is, pstate, &_top);
    if (_state == PP_COMPLETE) {
      _state = 0;
      _top = _top->next();
      if (!_top) {
        return;
      }
    }
  }
  if (_state < 0) {
    DBG_PULSE("PulseProcChain: closing input stream \n");
    // TODO Think about PAUSE state
    if (_is) _is->close();
  }
}

void PulseProcChain::loadTap(InputStream *is) {
  DBG_PULSE("PulseProcChain::loadTap \n");
  _ppTap.init(&_ppTap);
  init(is, &_ppTap);
}
