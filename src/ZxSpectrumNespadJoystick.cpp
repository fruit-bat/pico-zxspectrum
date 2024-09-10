#include <pico/printf.h>
#include "ZxSpectrumNespadJoystick.h"
#include "nespad.h"

ZxSpectrumNespadJoystick::ZxSpectrumNespadJoystick() :
  _kempston(0),
  _sinclairL(0xff),
  _sinclairR(0xff)
{
    // TODO static
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
