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
  tusb_hid_simple_joystick_t* simple_joysticks[2];
  return tuh_hid_get_simple_joysticks(simple_joysticks, 2) > 0;
}

bool ZxSpectrumHidMouseJoystick::isConnectedR() {
  tusb_hid_simple_joystick_t* simple_joysticks[2];
  return tuh_hid_get_simple_joysticks(simple_joysticks, 2) > 1;
}

void ZxSpectrumHidMouseJoystick::decode() {
  tusb_hid_simple_joystick_t* simple_joysticks[2];
  uint8_t n = tuh_hid_get_simple_joysticks(simple_joysticks, 2);
  if (n > 0) {
    tusb_hid_simple_joystick_t* joystick = simple_joysticks[0];
    if (_updated1 != joystick->updated) {
      uint8_t kempston = 0;
      tusb_hid_simple_joystick_values_t* values = &joystick->values;
      // 000FUDLR

      if (mode() == ZxSpectrumJoystickModeSinclairRL) {
        uint8_t sinclairR = 0xff;
        if (values->x1 == joystick->axis_x1.logical_max || values->x2 == joystick->axis_x2.logical_max) {
          kempston |= 1<<0;
          sinclairR &= ~(1<<3);
        }
        if (values->x1 == joystick->axis_x1.logical_min || values->x2 == joystick->axis_x2.logical_min) {
          kempston |= 1<<1;
          sinclairR &= ~(1<<4);
        }	
        if (values->y1 == joystick->axis_y1.logical_max || values->y2 == joystick->axis_y2.logical_max) {
          kempston |= 1<<2;
          sinclairR &= ~(1<<2);
        }
        if (values->y1 == joystick->axis_y1.logical_min || values->y2 == joystick->axis_y2.logical_min) {
          kempston |= 1<<3;
          sinclairR &= ~(1<<1);
        }
        if (values->buttons & 7) {
          //kempston |= 1<<4;
          sinclairR &= ~(1<<0);
        }
        _sinclairR = sinclairR;
        // 4BK - 4 Buttons Kempston Joystick
        kempston |= (values->buttons & 14)<<3;   // 4 Button Kempston | |X|A|B|.|DDDD|
        kempston |= (values->buttons &  1)<<7;   // 4 Button Kempston |Y| | | |.|DDDD|
      }
      else {
        uint8_t sinclairL = 0xff;
        if (values->x1 == joystick->axis_x1.logical_max || values->x2 == joystick->axis_x2.logical_max) {
          kempston |= 1<<0;
          sinclairL &= ~(1<<1);
        }
        if (values->x1 == joystick->axis_x1.logical_min || values->x2 == joystick->axis_x2.logical_min) {
          kempston |= 1<<1;
          sinclairL &= ~(1<<0);
        }	
        if (values->y1 == joystick->axis_y1.logical_max || values->y2 == joystick->axis_y2.logical_max) {
          kempston |= 1<<2;
          sinclairL &= ~(1<<2);
        }
        if (values->y1 == joystick->axis_y1.logical_min || values->y2 == joystick->axis_y2.logical_min) {
          kempston |= 1<<3;
          sinclairL &= ~(1<<3);
        }
        if (values->buttons & 7) {
          //kempston |= 1<<4;                     //Kempstone 1 Button Fire = (ABY)
            sinclairL &= ~(1<<4);
        }
        _sinclairL = sinclairL;
        // 4BK - 4 Buttons Kempston Joystick
        kempston |= (values->buttons & 14)<<3;   // 4 Button Kempston | |X|A|B|.|DDDD|
        kempston |= (values->buttons &  1)<<7;   // 4 Button Kempston |Y| | | |.|DDDD|
      }
      _kempston = kempston;
      _updated1 = joystick->updated;
    }
  }
  if (n > 1) {
    tusb_hid_simple_joystick_t* joystick = simple_joysticks[1];
    if (_updated2 != joystick->updated) {

      tusb_hid_simple_joystick_values_t* values = &joystick->values;
      if (mode() == ZxSpectrumJoystickModeSinclairRL) {
        uint8_t sinclairL = 0xff;
        if (values->x1 == joystick->axis_x1.logical_max || values->x2 == joystick->axis_x2.logical_max) {
          sinclairL &= ~(1<<1);
        }
        if (values->x1 == joystick->axis_x1.logical_min || values->x2 == joystick->axis_x2.logical_min) {
          sinclairL &= ~(1<<0);
        }	
        if (values->y1 == joystick->axis_y1.logical_max || values->y2 == joystick->axis_y2.logical_max) {
          sinclairL &= ~(1<<2);
        }
        if (values->y1 == joystick->axis_y1.logical_min || values->y2 == joystick->axis_y2.logical_min) {
          sinclairL &= ~(1<<3);
        }
        if (values->buttons & 7) {
          sinclairL &= ~(1<<4);
        }
        _sinclairL = sinclairL;
      }
      else {
        uint8_t sinclairR = 0xff;
        if (values->x1 == joystick->axis_x1.logical_max || values->x2 == joystick->axis_x2.logical_max) {
          sinclairR &= ~(1<<3);
        }
        if (values->x1 == joystick->axis_x1.logical_min || values->x2 == joystick->axis_x2.logical_min) {
          sinclairR &= ~(1<<4);
        }	
        if (values->y1 == joystick->axis_y1.logical_max || values->y2 == joystick->axis_y2.logical_max) {
          sinclairR &= ~(1<<2);
        }
        if (values->y1 == joystick->axis_y1.logical_min || values->y2 == joystick->axis_y2.logical_min) {
          sinclairR &= ~(1<<1);
        }
        if (values->buttons & 7) {
          sinclairR &= ~(1<<0);
        }
        _sinclairR = sinclairR;
      }
      _updated2 = joystick->updated;
    }
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
    // Buttons	D0:Right D1:Left D2:Middle
    _buttons = (
        (b & MOUSE_BUTTON_LEFT ? 2 : 0) | 
        (b & MOUSE_BUTTON_MIDDLE ? 4 : 0) |
        (b & MOUSE_BUTTON_RIGHT ? 1 : 0)
    );
}
