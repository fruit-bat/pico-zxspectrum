#include <pico/printf.h>
#include "hid_host_joy.h"
#include "ZxSpectrumMatrixJoystick.h"
#include "ZxSpectrumKeyMatrix.h"

ZxSpectrumMatrixJoystick::ZxSpectrumMatrixJoystick() :
  _enabled(true)
{
}

uint8_t ZxSpectrumMatrixJoystick::sinclairL() {
  return 0xff;
}

uint8_t ZxSpectrumMatrixJoystick::sinclairR() {
  return 0xff;
}

uint8_t ZxSpectrumMatrixJoystick::kempston() {
  return _enabled ? zx_kempston() : 0;
}

uint8_t ZxSpectrumMatrixJoystick::joy1(){ 
  return 0; 
};
