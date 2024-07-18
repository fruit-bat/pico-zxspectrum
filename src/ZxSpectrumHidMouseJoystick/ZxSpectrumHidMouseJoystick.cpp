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
  //_us(time_us_32()),
  _xp(0),
  _yp(0),
    //-----------
  left(false),
  right(false),
  up(false),
  down(false)
  //-----------
{
}

bool ZxSpectrumHidMouseJoystick::isConnectedL() {
  return _mounted > 0;
}

bool ZxSpectrumHidMouseJoystick::isConnectedR() {
  return false;
}


//#define AXIS_THRESHOLD_LOG2 4
//#define AXIS_THRESHOLD (1<<AXIS_THRESHOLD_LOG2)
//--------------------------------------------------
#define AXIS_THRESHOLD  32      // 4/4
#define AXIS_RELEASE    24      // 3/4
#define AXIS_EDGE       16      // 2/4    
//==================================================
void ZxSpectrumHidMouseJoystick::decode() {
    uint8_t kempston = 0;
    uint8_t sinclairL = 0xff;
    uint8_t sinclairR = 0xff;

    //VAR.2---------------------------------------------
    // X Axis Control -----------------------------------
    if (_yAcc > _yp+AXIS_EDGE || _yAcc < _yp-AXIS_EDGE) {
        //RIGHT in the corner of the quadrant
        if (_xAcc > _xp+AXIS_THRESHOLD) {
            if (_yAcc > _yp+AXIS_EDGE) {_yp++;} else {_yp--;}  //Bevel    
            _xp = _xAcc-AXIS_THRESHOLD;
            right = true;
        } else if (_xAcc < _xp+AXIS_RELEASE) {right = false;}  //Bevel   
        //LEFT in the corner of the quadrant
        if (_xAcc < _xp-AXIS_THRESHOLD) {
            if (_yAcc > _yp+AXIS_EDGE) {_yp++;} else {_yp--;}
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
    if (_xAcc > _xp+AXIS_EDGE || _xAcc < _xp-AXIS_EDGE) {
        //UP in the corner of the quadrant
        if (_yAcc > _yp+AXIS_THRESHOLD) {
            if (_xAcc > _xp+AXIS_EDGE) {_xp++;} else {_xp--;}  //Bevel
            _yp = _yAcc-AXIS_THRESHOLD;
            up = true;
        } else if (_yAcc < _yp+AXIS_RELEASE) {up = false;}
        //DOWN in the corner of the quadrant
        if (_yAcc < _yp-AXIS_THRESHOLD) {
            if (_xAcc > _xp+AXIS_EDGE) {_xp++;} else {_xp--;}
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

    //VAR.1---------------------------------------------
    /*
    if (right) {
        if (_xAcc > _xp+AXIS_THRESHOLD) {_xp=_xAcc-AXIS_THRESHOLD;}
        if (_xAcc < _xp+AXIS_RELEASE) {right = false;}
    } else {
        if (_xAcc > _xp+AXIS_THRESHOLD) {right = true;}
    }
    if (left) {
        if (_xAcc < _xp-AXIS_THRESHOLD) {_xp=_xAcc+AXIS_THRESHOLD;}
        if (_xAcc > _xp-AXIS_RELEASE) {left = false;}
    } else {
        if (_xAcc < _xp-AXIS_THRESHOLD) {left = true;}
    }
    if (up) {
        if (_yAcc > _yp+AXIS_THRESHOLD) {_yp=_yAcc-AXIS_THRESHOLD;}
        if (_yAcc < _yp+AXIS_RELEASE) {up = false;}
    } else {
        if (_yAcc > _yp+AXIS_THRESHOLD) {up = true;}
    }
    if (down) {
        if (_yAcc < _yp-AXIS_THRESHOLD) {_yp=_yAcc+AXIS_THRESHOLD;}
        if (_yAcc > _yp-AXIS_RELEASE) {down = false;}
    } else {
        if (_yAcc < _yp-AXIS_THRESHOLD) {down = true;}
    }
    */

    //---------------------------------------------
    //Buttons control
    kempston |= _buttons << 4;      // 4BK 
    //kempston |= _wAcc & 128;

    if (_buttons & MOUSE_BUTTON_LEFT) {
        //kempston |= 1 << 4;
        if (_isLeft) {
            sinclairL &= ~(1<<4);
        }
        else { 
            sinclairR &= ~(1<<0);
        }
    }

    //---------------------------------------------

    if (right) { // Right
        kempston |= 1<<0;
        if (_isLeft) {
            sinclairL &= ~(1<<1);
        }
        else {
            sinclairR &= ~(1<<3);
        }
    }
    if (left) { // Left
        kempston |= 1<<1;
        if (_isLeft) {
            sinclairL &= ~(1<<0);
        }
        else {
            sinclairR &= ~(1<<4);
        }
    }
    if (up) { // Up
        kempston |= 1<<3;
        if (_isLeft) {
            sinclairL &= ~(1<<3);
        }
        else {
            sinclairR &= ~(1<<1);
        }
    }
    if (down) { // Down
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
