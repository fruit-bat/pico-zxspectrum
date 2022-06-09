#include <pico/printf.h>
#include "hid_host_joy.h"
#include "ZxSpectrumPicomputerJoystick.h"
#include "pzx_keyscan.h"

ZxSpectrumPicomputerJoystick::ZxSpectrumPicomputerJoystick() :
  _enabled(true)
{
}

uint8_t ZxSpectrumPicomputerJoystick::sinclairL() {
  return 0xff;
}

uint8_t ZxSpectrumPicomputerJoystick::sinclairR() {
  return 0xff;
}

uint8_t ZxSpectrumPicomputerJoystick::kempston() {
  return _enabled ? pzx_kempston() : 0;
}
