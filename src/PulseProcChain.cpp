#include "PulseProcChain.h"

PulseProcChain::PulseProcChain() :
  _is(0),
  _top(0),
  _state(PP_COMPLETE),
  _acc(0),
  _ppTone1(),
  _ppStdByte(&_ppTone1),
  _ppStdByteStream(&_ppStdByte),
  _ppStdHeader(&_ppTone1),
  _ppPause(),
  _ppTap(&_ppStdHeader, &_ppStdByte, &_ppStdByteStream, &_ppPause),
  _ppTzx(&_ppTap, &_ppStdHeader, &_ppStdByteStream, &_ppTone1, &_ppPause)
{
}

void PulseProcChain::reset() {
  DBG_PULSE("PulseProcChain: reset \n");
  _is = 0;
  _top = 0;
  _state = PP_COMPLETE;
  _acc = 0;
}

void PulseProcChain::init(InputStream *is, PulseProc* top) {
  DBG_PULSE("PulseProcChain: init \n");
  _is = is;
  _top = top;
  _state = PP_CONTINUE;
  _acc = 0;
}

void PulseProcChain::pause(bool pause) {
  if (pause) {
    if (_state >= PP_CONTINUE) {
      _state = PP_PAUSE; 
    }
  }
  else {
    if (_state == PP_PAUSE) {
      _state = PP_CONTINUE; 
    }
  }
}

void __not_in_flash_func(PulseProcChain::advance)(uint32_t tstates, bool *pstate) {
  if (_state < 0) return;
  _acc += tstates;
  while (_state >= 0 && _acc >= static_cast<uint32_t>(_state)) {
    _acc -= _state;
    _state = _top->advance(_is, pstate, &_top);
    if (_state == PP_COMPLETE) {
      _state = 0;
      _top = _top ? _top->next() : 0;
      if (!_top) { 
        _state = PP_COMPLETE;
        break;
      }
    }
  }
  if (_state < 0 && _state != PP_PAUSE) {
    DBG_PULSE("PulseProcChain: closing input stream \n");
    if (_is) _is->close();
  }
}

void PulseProcChain::loadTap(
    InputStream *is,
    uint32_t tsPerMs
) {
  DBG_PULSE("PulseProcChain::loadTap \n");
  if (is) {
    _ppTap.init(&_ppTap, 1000, tsPerMs);
    init(is, &_ppTap);
  }
  else {
    reset();
  }
}

void PulseProcChain::loadTzx(
  InputStream *is,
  uint32_t tsPerMs,
  bool is48k
) {
  DBG_PULSE("PulseProcChain::loadTzx Partially implemented \n");
  if (is) {
    _ppTzx.init(0, tsPerMs, is48k);
    init(is, &_ppTzx);
  }
  else {
    reset();
  }
}

void PulseProcChain::option(uint32_t option) {
  DBG_PULSE("PulseProcChain:option %ld\n", option);
  if (_state == PP_PAUSE) {
    _ppTzx.option(option);
    pause(false);
  }
}


