#pragma once

#include <pico/printf.h>
#include "ZxSpectrumJoystick.h"
#include "ZxSpectrumMouse.h"

class ZxSpectrumKeyboard {
  
  unsigned char _line[8];
  unsigned char _vline[8]; //virtual keyboard

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
    for(int i = 0; i < 8; ++i) _vline[i] = 0xff;
    reset();
  }

  inline void reset() {
    for(int i = 0; i < 8; ++i) _line[i] = 0xff;
  }

  inline void press(uint8_t line, uint8_t keymask) {
    _line[line] &= ~keymask; 
  }

  inline void release(uint8_t line, uint8_t keymask) {
    _line[line] |= keymask;
  }

  inline void virtuallpress(uint8_t line, uint8_t keymask) {
    _vline[line] &= ~keymask; 
  }

/*
Port 	  Dec 	   Bin 	Address line 	              D0 	D1 	D2 	D3 	D4
$fdfe 	65022 	 %1111 1101 1111 1110 	A9 	  A 	S 	D 	F 	G
$fbfe 	64510 	 %1111 1011 1111 1110 	A10 	Q 	W 	E 	R 	T
$fefe 	65278 	 %1111 1110 1111 1110 	A8 	Caps shift 	Z 	X 	C 	V
$f7fe 	63486 	 %1111 0111 1111 1110 	A11 	1 	2 	3 	4 	5
$effe 	61438 	 %1110 1111 1111 1110 	A12 	0 	9 	8 	7 	6
$dffe 	57342 	 %1101 1111 1111 1110 	A13 	P 	O 	I 	U 	Y
$bffe 	49150 	 %1011 1111 1111 1110 	A14 	Ent 	L 	K 	J 	H
$7ffe 	32766 	 %0111 1111 1111 1110 	A15 	Spc 	Sym shft 	M 	N 	B 
*/
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
      if (rs & (1 << i)) { 
        a &= _line[i] & _vline[i];
        _vline[i] = 0xff;
      }
    }
    //if (a != 0xff) printf("input %04X %02X\n", address, a);
    return a;
  }

  virtual bool isMounted() { return false; }
};
