#include "ZxSpectrumJoystick.h"

uint8_t ZxSpectrumJoystick::getKempston() { 
  return _mode == ZxSpectrumJoystickModeKempston ? kempston() : 0; 
}

uint8_t ZxSpectrumJoystick::getSinclairL() {
  switch(_mode) {
    case ZxSpectrumJoystickModeSinclairLR:
    case ZxSpectrumJoystickModeSinclairRL:
    return sinclairL();
    default: return 0xff; 
  }
}

uint8_t ZxSpectrumJoystick::getSinclairR() { 
  switch(_mode) {
    case ZxSpectrumJoystickModeSinclairLR:
    case ZxSpectrumJoystickModeSinclairRL:
    return sinclairR();
    default: return 0xff; 
  }
}
