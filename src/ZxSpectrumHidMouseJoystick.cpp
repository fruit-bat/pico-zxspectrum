#include <pico/printf.h>
#include "tusb.h"
#include "hid_host_joy.h"
#include "ZxSpectrumHidMouseJoystick.h"

ZxSpectrumHidMouseJoystick::ZxSpectrumHidMouseJoystick() :
  _kempston(0),
  _sinclairL(0xff),
  _sinclairR(0xff),
  _isLeft(true),
  _xAcc(0),
  _yAcc(0),
  _wAcc(0),
  _mounted(0),
  _us(time_us_32())
{
}

bool ZxSpectrumHidMouseJoystick::isConnectedL() {
  return _mounted > 0;
}

bool ZxSpectrumHidMouseJoystick::isConnectedR() {
  return false;
}

#define AXIS_THRESHOLD (1<<4)

void ZxSpectrumHidMouseJoystick::decode() {
    uint8_t kempston = 0;
    uint8_t sinclairL = 0xff;
    uint8_t sinclairR = 0xff;
    uint32_t us = time_us_32();
    if (_buttons & MOUSE_BUTTON_LEFT) {
        kempston |= 1 << 4;
        if (_isLeft) {
            sinclairL &= ~(1<<4);
        }
        else {
            sinclairR &= ~(1<<0);
        }
    }
    if (_xAcc > AXIS_THRESHOLD) { // Right
        kempston |= 1<<0;
        if (_isLeft) {
            sinclairL &= ~(1<<1);
        }
        else {
            sinclairR &= ~(1<<3);
        }
    }
    if (_xAcc < -AXIS_THRESHOLD) { // Left
        kempston |= 1<<1;
        if (_isLeft) {
            sinclairL &= ~(1<<0);
        }
        else {
            sinclairR &= ~(1<<4);
        }
    }
    if (_yAcc > AXIS_THRESHOLD) { // Up
        kempston |= 1<<3;
        if (_isLeft) {
            sinclairL &= ~(1<<3);
        }
        else {
            sinclairR &= ~(1<<1);
        }
    }
    if (_yAcc < -AXIS_THRESHOLD) { // Down
        kempston |= 1<<2;
        if (_isLeft) {
            sinclairL &= ~(1<<2);
        }
        else {
            sinclairR &= ~(1<<2);
        }
    } 
    _kempston = kempston;
    _sinclairL = sinclairL;
    _sinclairR = sinclairR;
    if (us - _us > 1000) {
        _xAcc >>= 1;
        _yAcc >>= 1;
        _wAcc >>= 1;      
        _us = us;
    }
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
    _buttons = (uint8_t)b;
}
