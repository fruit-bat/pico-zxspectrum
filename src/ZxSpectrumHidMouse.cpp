#include "ZxSpectrumHidMouse.h"
#include "hid_host_info.h"

ZxSpectrumHidMouse::ZxSpectrumHidMouse() : _mounted(0) {
    _reset();
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::buttons)()
{
    if (mouseMode() != ZxSpectrumMouseModeKempstonMouse) return 0xff;

    const uint32_t buttons = 
        (_buttons & MOUSE_BUTTON_LEFT ? 2 : 0) | 
        (_buttons & MOUSE_BUTTON_MIDDLE ? 4 : 0) |
        (_buttons & MOUSE_BUTTON_RIGHT ? 1 : 0);

    return 0xff & ~(_wAcc << 4 | buttons);
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::xAxis)()
{
    return mouseMode() == ZxSpectrumMouseModeKempstonMouse ? _xAcc >> 3 : 0;
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::yAxis)()
{
    return mouseMode() == ZxSpectrumMouseModeKempstonMouse ? _yAcc >> 3 : 0;
}

bool ZxSpectrumHidMouse::isConnectedL() {
  return mouseMode() == ZxSpectrumMouseModeJoystick ? _mounted > 0 && mode() == ZxSpectrumJoystickModeSinclairLR : false;
}

bool ZxSpectrumHidMouse::isConnectedR() {
  return mouseMode() == ZxSpectrumMouseModeJoystick ? _mounted > 0 && mode() == ZxSpectrumJoystickModeSinclairRL : false;
}

#define AXIS_THRESHOLD  32      // 4/4
#define AXIS_RELEASE    24      // 3/4
#define AXIS_EDGE       16      // 2/4    
//==================================================
void ZxSpectrumHidMouse::decode() {
    uint8_t kempston = 0;
    uint8_t sinclairL = 0xff;
    uint8_t sinclairR = 0xff;

    //VAR.3---------------------------------------------
    // X Axis Control -----------------------------------
    if (_yAcc > _yp+AXIS_RELEASE || _yAcc < _yp-AXIS_RELEASE) {
        //RIGHT in the corner of the quadrant
        if (_xAcc > _xp+AXIS_THRESHOLD) {
            _xp = _xAcc-AXIS_THRESHOLD;
            right = true;
        } else if (_xAcc < _xp+AXIS_RELEASE) {right = false;}    
        //LEFT in the corner of the quadrant
        if (_xAcc < _xp-AXIS_THRESHOLD) {
            _xp = _xAcc+AXIS_THRESHOLD;
            left = true;
        } else if (_xAcc > _xp-AXIS_RELEASE) {left = false;}
    } else {
        //RIGHT in the central strip
        if (_xAcc > _xp+AXIS_THRESHOLD) {
            _xp = _xAcc-AXIS_THRESHOLD;
            right = true; up = false; down = false;
        } else if (_xAcc < _xp+AXIS_RELEASE) {right = false;}
        //LEFT in the central strip
        if (_xAcc < _xp-AXIS_THRESHOLD) {
            _xp = _xAcc+AXIS_THRESHOLD;
            left = true; up = false; down = false;
        } else if (_xAcc > _xp-AXIS_RELEASE) {left = false;}
    }
    // Y Axis Control -----------------------------------
    if (_xAcc > _xp+AXIS_RELEASE || _xAcc < _xp-AXIS_RELEASE) {
        //UP in the corner of the quadrant
        if (_yAcc > _yp+AXIS_THRESHOLD) {
            _yp = _yAcc-AXIS_THRESHOLD;
            up = true;
        } else if (_yAcc < _yp+AXIS_RELEASE) {up = false;}
        //DOWN in the corner of the quadrant
        if (_yAcc < _yp-AXIS_THRESHOLD) {
            _yp = _yAcc+AXIS_THRESHOLD;
            down = true;
        } else if (_yAcc > _yp-AXIS_RELEASE) {down = false;}
    } else {
        //UP in the central strip
        if (_yAcc > _yp+AXIS_THRESHOLD) {
            _yp = _yAcc-AXIS_THRESHOLD;
            up = true; right = false; left = false;
        } else if (_yAcc < _yp+AXIS_RELEASE) {up = false;}
        //DOWN in the central strip
        if (_yAcc < _yp-AXIS_THRESHOLD) {
            _yp = _yAcc+AXIS_THRESHOLD;
            down = true; right = false; left = false;
        } else if (_yAcc > _yp-AXIS_RELEASE) {down = false;}
    }

    //---------------------------------------------
    //Buttons control
    kempston |= _buttons << 4;      // 4BK 
    //kempston |= _wAcc & 128;
    const bool isLeft = mode() == ZxSpectrumJoystickModeSinclairLR;

    if (_buttons & MOUSE_BUTTON_LEFT) {
        //kempston |= 1 << 4;
        if (isLeft) {
            sinclairL &= ~(1<<4);
        }
        else { 
            sinclairR &= ~(1<<0);
        }
    }

    //---------------------------------------------

    if (right) { // Right
        kempston |= 1<<0;
        if (isLeft) {
            sinclairL &= ~(1<<1);
        }
        else {
            sinclairR &= ~(1<<3);
        }
    }
    if (left) { // Left
        kempston |= 1<<1;
        if (isLeft) {
            sinclairL &= ~(1<<0);
        }
        else {
            sinclairR &= ~(1<<4);
        }
    }
    if (up) { // Up
        kempston |= 1<<3;
        if (isLeft) {
            sinclairL &= ~(1<<3);
        }
        else {
            sinclairR &= ~(1<<1);
        }
    }
    if (down) { // Down
        kempston |= 1<<2;
        if (isLeft) {
            sinclairL &= ~(1<<2);
        }
        else {
            sinclairR &= ~(1<<2);
        }
    } 
    _kempston = kempston;
    _sinclairL = sinclairL;
    _sinclairR = sinclairR;
}

uint8_t ZxSpectrumHidMouse::sinclairL()
{
    if (mouseMode() != ZxSpectrumMouseModeJoystick) return 0xff;
    decode();
    return _sinclairL;
}

uint8_t ZxSpectrumHidMouse::sinclairR()
{
    if (mouseMode() != ZxSpectrumMouseModeJoystick) return 0xff;
    decode();
    return _sinclairR;
}

uint8_t ZxSpectrumHidMouse::kempston()
{
    if (mouseMode() != ZxSpectrumMouseModeJoystick) return 0x00;
    decode();
    return _kempston;
}

void __not_in_flash_func(ZxSpectrumHidMouse::setButtons)(uint32_t b)
{
    _buttons = (uint8_t)b;
}

// non-virtual version of reset so it can be called from constructor
void ZxSpectrumHidMouse::_reset() {
    // Mouse stuff
    _xAcc = 0;
    _yAcc = 0;
    _wAcc = 0;
    _buttons = 0;
    // Joystick stuff
    _kempston = 0;
    _sinclairL = 0;
    _sinclairR = 0;
   _xp = 0;
   _yp = 0;
   left = false;
   right = false;
   up = false;
   down = false;    
}

void ZxSpectrumHidMouse::reset() {
    _reset();
}

void ZxSpectrumHidMouse::mouseMode(ZxSpectrumMouseMode mode) {
    ZxSpectrumMouse::mouseMode(mode);
    reset();
}
