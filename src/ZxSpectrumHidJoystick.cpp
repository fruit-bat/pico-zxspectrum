#include <pico/printf.h>
#include "tusb.h"
#include "hid_host_joy.h"
#include "ZxSpectrumHidJoystick.h"

ZxSpectrumHidJoystick::ZxSpectrumHidJoystick() :
  _updatedL(0),
  _updatedR(0),
  _kempston(0),
  _sinclairL(0xff),
  _sinclairR(0xff)
{
}

bool ZxSpectrumHidJoystick::isConnectedL() {
  tusb_hid_simple_joysick_t* simple_joysticks[2];
  return tuh_hid_get_simple_joysticks(simple_joysticks, 2) > 0;
}

bool ZxSpectrumHidJoystick::isConnectedR() {
  tusb_hid_simple_joysick_t* simple_joysticks[2];
  return tuh_hid_get_simple_joysticks(simple_joysticks, 2) > 1;
}

void ZxSpectrumHidJoystick::decode() {
  tusb_hid_simple_joysick_t* simple_joysticks[2];
  uint8_t n = tuh_hid_get_simple_joysticks(simple_joysticks, 2);
  if (n > 0) {
    tusb_hid_simple_joysick_t* joystick = simple_joysticks[0];
    if (_updatedL != joystick->updated) {
      uint8_t kempston = 0;
      uint8_t sinclairL = 0xff;
      tusb_hid_simple_joysick_values_t* values = &joystick->values;
      // 000FUDLR
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
      //multi button interfacing
      //the SEGA 3 button magic smoke effect limits to 2 pysical buttons
      //very specific to clockwise button order cheap controllers
      // BFUDLER interface
      if(values->buttons & (16|32)) {
        //button shoulders
        uint8_t k = kempston;
        //use direction summand zeros
        //but no diagonals
        if((k & 3) == 0) kempston |= 3;
        if((k & (3<<2)) == 0) kempston |= (3<<2);
        if(k == kempston) kempston |= 15;//button priority
      }
      if(values->buttons & 1) {
        //button X
        kempston = 3;//LR
      }
      if(values->buttons & 8) {
        //button Y
        kempston = 12;//UD
      }
      if(values->buttons & 2) {
        //button A
        kempston |= 1<<4;//main fire button
      }
      if(values->buttons & 4) {
        //button B
        kempston |= 1<<5;//aux fire button
      }
      //and sinclair do usual
      if (values->buttons & 7) {
        sinclairL &= ~(1<<4);
      }

      _kempston = kempston;
      _sinclairL = sinclairL;
      _updatedL = joystick->updated;
    }
  }
  if (n > 1) {
    tusb_hid_simple_joysick_t* joystick = simple_joysticks[1];
    if (_updatedR != joystick->updated) {
      uint8_t sinclairR = 0xff;
      tusb_hid_simple_joysick_values_t* values = &joystick->values;
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
      _updatedR = joystick->updated;
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
