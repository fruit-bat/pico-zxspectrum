#include <pico/printf.h>
#include "tusb.h"
#include "hid_host_joy.h"
#include "ZxSpectrumHidMouseJoystick.h"

ZxSpectrumHidMouseJoystick::ZxSpectrumHidMouseJoystick() :
  _updated1(0),
  _updated2(0),
  _kempston(0),
  _sinclairL(0xff),
  _sinclairR(0xff)
{
}

bool ZxSpectrumHidMouseJoystick::isConnectedL() {
  return _mounted > 0;
}

bool ZxSpectrumHidMouseJoystick::isConnectedR() {
  return false;
}

void ZxSpectrumHidMouseJoystick::decode() {

}

uint8_t ZxSpectrumHidMouseJoystick::sinclairL() {
  decode();
  return _sinclairL;
}

uint8_t ZxSpectrumHidMouseJoystick::sinclairR() {
  decode();
  return _sinclairR;
}

uint8_t ZxSpectrumHidMouseJoystick::kempston() {
  decode();
  return _kempston;
}

void __not_in_flash_func(ZxSpectrumHidMouseJoystick::setButtons)(uint32_t b)
{
    // Buttons	D0:Right D1:Left D2:Middle
}
