#include <pico/printf.h>
#include "tusb.h"
#include "hid_host_joy.h"
#include "ZxSpectrumHidJoystick.h"

uint8_t ZxSpectrumHidJoystick::kempstone() {
  uint8_t data = 0;
  tusb_hid_simple_joysick_t* simple_joysticks[1];
  if (tuh_hid_get_simple_joysticks(simple_joysticks, 1) > 0) {
    tusb_hid_simple_joysick_t* joystick = simple_joysticks[0];
    tusb_hid_simple_joysick_values_t* values = &joystick->values;
    // 000FUDLR
    if (values->x1 == joystick->axis_x1.logical_max) data |= 1<<0;
    if (values->x1 == joystick->axis_x1.logical_min) data |= 1<<1;
    if (values->y1 == joystick->axis_y1.logical_max) data |= 1<<2;
    if (values->y1 == joystick->axis_y1.logical_min) data |= 1<<3;
    if (values->x2 == joystick->axis_x2.logical_max) data |= 1<<0;
    if (values->x2 == joystick->axis_x2.logical_min) data |= 1<<1;
    if (values->y2 == joystick->axis_y2.logical_max) data |= 1<<2;
    if (values->y2 == joystick->axis_y2.logical_min) data |= 1<<3;    
    if (values->buttons & 7) data |= 1<<4;
  }
  return data; 
}
