#include <pico/printf.h>
#include "tusb.h"
#include "hid_host_joy.h"
#include "ZxSpectrumDualJoystick.h"

ZxSpectrumDualJoystick::ZxSpectrumDualJoystick(
    ZxSpectrumJoystick *joystick1,
    ZxSpectrumJoystick *joystick2
) :
  _joystick1(joystick1),
  _joystick2(joystick2)
{
}

uint8_t ZxSpectrumDualJoystick::sinclairL() {
  return _joystick1->sinclairL() & _joystick2->sinclairL();
}

uint8_t ZxSpectrumDualJoystick::sinclairR() {
  return _joystick1->sinclairR() & _joystick2->sinclairR();
}

uint8_t ZxSpectrumDualJoystick::kempston() {
  return _joystick1->kempston() | _joystick2->kempston();
}
