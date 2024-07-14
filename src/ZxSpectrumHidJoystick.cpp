#include <pico/printf.h>
#include "tusb.h"
#include "hid_host_joy.h"
#include "ZxSpectrumHidJoystick.h"

ZxSpectrumHidJoystick::ZxSpectrumHidJoystick() :
  _updated1(0),
  _updated2(0),
  _kempston(0),
  _sinclairL(0xff),
  _sinclairR(0xff)
{
}

bool ZxSpectrumHidJoystick::isConnectedL() {
  tusb_hid_simple_joystick_t* simple_joysticks[2];
  return tuh_hid_get_simple_joysticks(simple_joysticks, 2) > 0;
}

bool ZxSpectrumHidJoystick::isConnectedR() {
  tusb_hid_simple_joystick_t* simple_joysticks[2];
  return tuh_hid_get_simple_joysticks(simple_joysticks, 2) > 1;
}

#define HAT_BITS_U 0b10000011L
#define HAT_BITS_D 0b00111000L
#define HAT_BITS_L 0b11100000L
#define HAT_BITS_R 0b00001110L

void ZxSpectrumHidJoystick::decode() {
  tusb_hid_simple_joystick_t* simple_joysticks[2];
  uint8_t n = tuh_hid_get_simple_joysticks(simple_joysticks, 2);
  if (n > 0) {
    tusb_hid_simple_joystick_t* joystick = simple_joysticks[0];
    if (_updated1 != joystick->updated) {
      uint8_t kempston = 0;
      tusb_hid_simple_joystick_values_t* values = &joystick->values;

      uint32_t hatm = 0;
      if (joystick->hat.length > 0) {
        uint32_t hat = (uint32_t)values->hat;
        for(uint32_t i = joystick->hat.logical_max + 1; i > 8; i = i >> 1, hat >>= 1);
        hatm = 1 << hat;
      }
      bool up = (values->y1 == joystick->axis_y1.logical_min || values->y2 == joystick->axis_y2.logical_min || !!(hatm & HAT_BITS_U));
      bool down = (values->y1 == joystick->axis_y1.logical_max || values->y2 == joystick->axis_y2.logical_max || !!(hatm & HAT_BITS_D));
      bool left = (values->x1 == joystick->axis_x1.logical_min || values->x2 == joystick->axis_x2.logical_min || !!(hatm & HAT_BITS_L));
      bool right = (values->x1 == joystick->axis_x1.logical_max || values->x2 == joystick->axis_x2.logical_max || !!(hatm & HAT_BITS_R));

      // 000FUDLR

      if (mode() == ZxSpectrumJoystickModeSinclairRL) {
        uint8_t sinclairR = 0xff;
        if (right) {
          kempston |= 1<<0;
          sinclairR &= ~(1<<3);
        }
        if (left) {
          kempston |= 1<<1;
          sinclairR &= ~(1<<4);
        }	
        if (down) {
          kempston |= 1<<2;
          sinclairR &= ~(1<<2);
        }
        if (up) {
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
        if (right) {
          kempston |= 1<<0;
          sinclairL &= ~(1<<1);
        }
        if (left) {
          kempston |= 1<<1;
          sinclairL &= ~(1<<0);
        }	
        if (down) {
          kempston |= 1<<2;
          sinclairL &= ~(1<<2);
        }
        if (up) {
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

      uint32_t hatm = 0;
      if (joystick->hat.length > 0) {
        uint32_t hat = (uint32_t)values->hat;
        for(uint32_t i = joystick->hat.logical_max + 1; i > 8; i = i >> 1, hat >>= 1);
        hatm = 1 << hat;
      }
      bool up = (values->y1 == joystick->axis_y1.logical_min || values->y2 == joystick->axis_y2.logical_min || !!(hatm & HAT_BITS_U));
      bool down = (values->y1 == joystick->axis_y1.logical_max || values->y2 == joystick->axis_y2.logical_max || !!(hatm & HAT_BITS_D));
      bool left = (values->x1 == joystick->axis_x1.logical_min || values->x2 == joystick->axis_x2.logical_min || !!(hatm & HAT_BITS_L));
      bool right = (values->x1 == joystick->axis_x1.logical_max || values->x2 == joystick->axis_x2.logical_max || !!(hatm & HAT_BITS_R));

      if (mode() == ZxSpectrumJoystickModeSinclairRL) {
        uint8_t sinclairL = 0xff;
        if (right) {
          sinclairL &= ~(1<<1);
        }
        if (left) {
          sinclairL &= ~(1<<0);
        }	
        if (down) {
          sinclairL &= ~(1<<2);
        }
        if (up) {
          sinclairL &= ~(1<<3);
        }
        if (values->buttons & 7) {
          sinclairL &= ~(1<<4);
        }
        _sinclairL = sinclairL;
      }
      else {
        uint8_t sinclairR = 0xff;
        if (right) {
          sinclairR &= ~(1<<3);
        }
        if (left) {
          sinclairR &= ~(1<<4);
        }	
        if (down) {
          sinclairR &= ~(1<<2);
        }
        if (up) {
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

uint8_t ZxSpectrumHidJoystick::sinclairL() {
  decode();
  return _sinclairL;
}

uint8_t ZxSpectrumHidJoystick::sinclairR() {
  decode();
  return _sinclairR;
}

uint8_t ZxSpectrumHidJoystick::kempston() {
  decode();
  return _kempston;
}
