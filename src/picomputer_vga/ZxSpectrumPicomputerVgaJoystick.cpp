#include <pico/printf.h>
#include "hid_host_joy.h"
#include "ZxSpectrumPicomputerVgaJoystick.h"
#include "pzx_keyscan.h"

ZxSpectrumPicomputerVgaJoystick::ZxSpectrumPicomputerVgaJoystick() :
  _enabled(true)
{
}

uint8_t ZxSpectrumPicomputerVgaJoystick::sinclairL() {
  return 0xff;
}

uint8_t ZxSpectrumPicomputerVgaJoystick::sinclairR() {
  return 0xff;
}

uint8_t ZxSpectrumPicomputerVgaJoystick::kempston() {
  return _enabled ? pzx_kempston() : 0;
}
