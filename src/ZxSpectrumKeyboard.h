#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"
#include "ZxSpectrumMouse.h"

class ZxSpectrumKeyboard {
  
  unsigned char _line[8];
  ZxSpectrumJoystick * _zxSpectrumJoystick;
  ZxSpectrumMouse * _zxSpectrumMouse;
public:
  ZxSpectrumKeyboard(
    ZxSpectrumJoystick * zxSpectrumJoystick,
    ZxSpectrumMouse * zxSpectrumMouse
  ) : 
    _zxSpectrumJoystick(zxSpectrumJoystick),
    _zxSpectrumMouse(zxSpectrumMouse)
  {
    reset();
  }

  inline void reset() {
    for(int i = 0; i < 8; ++i) _line[i] = 0xff;
  }

  inline void press(unsigned int line, unsigned int key) {
    _line[line] &= ~(1 << key);
  }

  inline void release(unsigned int line, unsigned int key) {
    _line[line] |= 1 << key;
  }

  unsigned char __not_in_flash_func(read)(int address) {
    int rs = ~(address >> 8);
    unsigned int a = 0xff;    
    const unsigned int b =  address | 0xf0;
    if (b == 0xf7fe) {
      if (_zxSpectrumJoystick) a = _zxSpectrumJoystick->getSinclairL();
      if (_zxSpectrumMouse) a &= _zxSpectrumMouse->getSinclairL();
    }
    if (b == 0xeffe) {
      if (_zxSpectrumJoystick) a = _zxSpectrumJoystick->getSinclairR();
      if (_zxSpectrumMouse) a &= _zxSpectrumMouse->getSinclairR();
    }

    for (int i = 0; i < 8; ++i) {
      if (rs & (1 << i)) a &= _line[i];
    }
    //if (a != 0xff) printf("input %04X %02X\n", address, a);
    return a;
  }
  
  virtual bool isMounted() { return false; }
};
