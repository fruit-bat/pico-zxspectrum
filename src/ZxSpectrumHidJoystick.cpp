#include <pico/printf.h>
#include "tusb.h"
#include "hid_host_joy.h"
#include "ZxSpectrumHidJoystick.h"

ZxSpectrumHidJoystick::ZxSpectrumHidJoystick() :
  _updated1(0),
  _updated2(0),
  _joy1(0),
  _joy2(0),
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

#define JOY_RIGHT 0x01
#define JOY_LEFT  0x02
#define JOY_DOWN  0x04
#define JOY_UP    0x08
#define JOY_BT1   0x10
#define JOY_BT2   0x20
#define JOY_BT3   0x40
#define JOY_BT0   0x80
#define JOY_FIRE  0x80

uint8_t ZxSpectrumHidJoystick::getjoystate(void * ptr) {
   tusb_hid_simple_joystick_t* joystick=(tusb_hid_simple_joystick_t*)ptr;
   uint8_t joy=0; //joystick state in 8bits
   
   tusb_hid_simple_joystick_values_t* values = &joystick->values;

   //Test direction on ASIX 1
   if (values->y1 == joystick->axis_y1.logical_min)  joy|=JOY_UP;       
     else if (values->y1 == joystick->axis_y1.logical_max) joy|=JOY_DOWN;

   if (values->x1 == joystick->axis_x1.logical_min)  joy|=JOY_LEFT;       
     else if (values->x1 == joystick->axis_x1.logical_max) joy|=JOY_RIGHT;

   if (joy==0) { //Test direction on ASIX 2
      if (values->y2 == joystick->axis_y2.logical_min)  joy|=JOY_UP;       
        else if (values->y2 == joystick->axis_y2.logical_max) joy|=JOY_DOWN;

      if (values->x2 == joystick->axis_x2.logical_min)  joy|=JOY_LEFT;       
        else if (values->x2 == joystick->axis_x2.logical_max) joy|=JOY_RIGHT;
   }

   if (joy==0) { //Test direction on hats
    uint32_t hatm = 0;
    if (joystick->hat.length > 0) {
        uint32_t hat = (uint32_t)values->hat;
        for(uint32_t i = joystick->hat.logical_max + 1; i > 8; i = i >> 1, hat >>= 1);
        hatm = 1 << hat;
    }
    if (!!(hatm & HAT_BITS_U)) joy|=JOY_UP;
      else if (!!(hatm & HAT_BITS_D)) joy|=JOY_DOWN;

    if (!!(hatm & HAT_BITS_L)) joy|=JOY_LEFT;
      else if (!!(hatm & HAT_BITS_R)) joy|=JOY_RIGHT;
   }

  joy |= (values->buttons & 0x0E)<<3;   // 4 Button joy | |3|2|1|.|DDDD|
  joy |= (values->buttons &  1)<<7;     // 4 Button joy |0| | | |.|DDDD|

  return joy;
}

#define SINCLAIR_RIGHT ~(1<<3)
#define SINCLAIR_LEFT  ~(1<<4)
#define SINCLAIR_DOWN  ~(1<<2)
#define SINCLAIR_UP    ~(1<<1)
#define SINCLAIR_FIRE  ~(1<<0)

uint8_t ZxSpectrumHidJoystick::joy2sinclair(uint8_t joy) {
  uint8_t sinclair = 0xff;

  if (joy & JOY_RIGHT) sinclair &= SINCLAIR_RIGHT;
      else if (joy & JOY_LEFT)  sinclair &= SINCLAIR_LEFT;
      
  if (joy & JOY_DOWN) sinclair &= SINCLAIR_DOWN;
     else  if  (joy & JOY_UP) sinclair &= SINCLAIR_UP;

  if (joy  & 0xB ) sinclair &= SINCLAIR_FIRE;

  return sinclair;
}

void ZxSpectrumHidJoystick::decode() {
  tusb_hid_simple_joystick_t* simple_joysticks[2];
  uint8_t n = tuh_hid_get_simple_joysticks(simple_joysticks, 2);
  if (n > 0) {
    tusb_hid_simple_joystick_t* joystick = simple_joysticks[0];
    if (_updated1 != joystick->updated) {

      _joy1=getjoystate(joystick);
      uint8_t sinclair=joy2sinclair(_joy1);

      if (mode() == ZxSpectrumJoystickModeSinclairRL) _sinclairR = sinclair;
        else  _sinclairL = sinclair;
      
      _kempston = _joy1;
      _updated1 = joystick->updated;
    }
  }

  if (n > 1) {
    tusb_hid_simple_joystick_t* joystick = simple_joysticks[1];
    if (_updated2 != joystick->updated) {
      _joy2=getjoystate(joystick);
      uint8_t sinclair=joy2sinclair(_joy2);

      if (mode() == ZxSpectrumJoystickModeSinclairRL) _sinclairL = sinclair;
        else _sinclairR = sinclair;
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

uint8_t ZxSpectrumHidJoystick::joy1() {
  decode();
  return _joy1;
}

uint8_t ZxSpectrumHidJoystick::joy2() {
  decode();
  return _joy2;
}
