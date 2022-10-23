#include "PulseProcStack.h"

PulseProcStack::PulseProcStack() :
  _is(0),
  _top(0),
  _state(PP_COMPLETE),
  _acc(0)
{
}

void PulseProcStack::init(InputStream *is, PulseProc* top) {
  _is = is;
  _top = top;
  _state = PP_CONTINUE;
  _acc = 0;
}

void __not_in_flash_func(PulseProcStack::advance)(uint32_t tstates, bool *pstate) {
  _acc += tstates;
  while (_state >= 0 && _acc > 0) {
    if (_acc >= static_cast<uint32_t>(_state)) {
      _acc -= _state;
      _state = _top->advance(_is, pstate, &_top);
      while (_state == PP_COMPLETE) {
        _top = _top->next();
        if (!_top) {
          return;
        }
        _state = _top->advance(_is, pstate, &_top);
      }
    }
  }
  if (_state < 0) {


  }
}
