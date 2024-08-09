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
    reset();
  }

  inline void reset() {
    for(int i = 0; i < 8; ++i) _vline[i] = _line[i] = 0xff;
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
#if 0
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
#endif

  unsigned char __not_in_flash_func(read)(int address) {
    uint8_t data = 0xff;    

    if (address == 0xfdfe) {       //A 	S 	D 	F 	G
        data &= _line[0];
        data &= _vline[0];
        _vline[0] = 0xFF; //autoreset after use?
    } else if (address == 0xfbfe) {//Q 	W 	E 	R 	T
        data &= _line[1];
        data &= _vline[1];
        _vline[1] = 0xFF;
    } else if (address == 0xfefe) {//Caps shift 	Z 	X 	C 	V
        data &= _line[2];
        data &= _vline[2];
        _vline[2] = 0xFF;
    } else if (address == 0xf7fe) {//1 	2 	3 	4 	5
        data &= _line[3];
        data &= _vline[3];
        _vline[3] = 0xFF;
        if (_zxSpectrumJoystick) data &= _zxSpectrumJoystick->getSinclairL();
        if (_zxSpectrumMouse) data &= _zxSpectrumMouse->getSinclairL();
    } else if (address == 0xeffe) {//0 	9 	8 	7 	6
        data &= _line[4]; 
        data &= _vline[4];
        _vline[4] = 0xFF;
        if (_zxSpectrumJoystick) data &= _zxSpectrumJoystick->getSinclairR();
        if (_zxSpectrumMouse) data &= _zxSpectrumMouse->getSinclairR();
    } else if (address == 0xdffe) {//P 	O 	I 	U 	Y
        data &= _line[5];
        data &= _vline[5];
        _vline[5] = 0xFF;
    } else if (address == 0xbffe) {//Ent 	L 	K 	J 	H
        data &= _line[6];
        data &= _vline[6];
        _vline[6] = 0xFF;
    } else if (address == 0x7ffe) {//Spc 	Sym shft 	M 	N 	B 
        data &= _line[7];
        data &= _vline[7];
        _vline[7] = 0xFF;

    }
    //if (data != 0xff) printf("input %04X %02X\n", address, data);
    return data;
}

  virtual bool isMounted() { return false; }
};
