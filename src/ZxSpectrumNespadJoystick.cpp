#include <pico/printf.h>
#include "ZxSpectrumNespadJoystick.h"
#include "nespad.h"

ZxSpectrumNespadJoystick::ZxSpectrumNespadJoystick() :
  _state(0),
  _kempston(0),
  _sinclairL(0xff),
  _sinclairR(0xff)
{
}

void ZxSpectrumNespadJoystick::init() {
   nespad_default_init();
}

bool ZxSpectrumNespadJoystick::isConnectedL() {
  return false;
}

bool ZxSpectrumNespadJoystick::isConnectedR() {
  return false;
}

void ZxSpectrumNespadJoystick::decode() {
    const uint32_t state = nespad_state();
    if (_state != state) {
      _sinclairL = nespad_to_sinclair_left(state, 0);
      _sinclairR = nespad_to_sinclair_right(state, 1);
      _kempston = nespad_to_kempston(state, 0);
      _state = state;
    }
}

uint8_t ZxSpectrumNespadJoystick::sinclairL() {
  decode();
  return _sinclairL;
}

uint8_t ZxSpectrumNespadJoystick::sinclairR() {
  decode();
  return _sinclairR;
}

uint8_t ZxSpectrumNespadJoystick::kempston() {
  decode();
  return _kempston;
}

uint8_t ZxSpectrumNespadJoystick::joy1() {
  decode();
  return _kempston;
}
