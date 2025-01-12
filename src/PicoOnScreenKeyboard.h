#pragma once
#include "PicoWin.h"
#include <string>
#include <functional>

class PicoOnScreenKeyboard : public PicoWin {

  std::function<void(uint8_t line , uint8_t key)> _press;
  std::string _chars;

 int8_t _x=0;
 int8_t _y=0;

#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08
#define BIT4  0x10

typedef struct  {
  uint8_t line;
  uint8_t key;
} ZxSpectrumKeyContact;

typedef struct {
  char  Name[4];
  uint8_t EscapeChar;
  ZxSpectrumKeyContact contact;
} ZxSpectrumKey;

const ZxSpectrumKey Line[4][10]={
    { 
      { " 1 ", 0, {3, BIT0} },
      { " 2 ", 0, {3, BIT1} },
      { " 3 ", 0, {3, BIT2} },
      { " 4 ", 0, {3, BIT3} },
      { " 5 ", 0, {3, BIT4} },
      { " 6 ", 0, {4, BIT4} },
      { " 7 ", 0, {4, BIT3} },
      { " 8 ", 0, {4, BIT2} },
      { " 9 ", 0, {4, BIT1} },
      { " 0 ", 0, {4, BIT0} }
    },
    { 
      { " Q ", 0, {2, BIT0} },
      { " W ", 0, {2, BIT1} },
      { " E ", 0, {2, BIT2} },
      { " R ", 0, {2, BIT3} },
      { " T ", 0, {2, BIT4} },
      { " Y ", 0, {5, BIT4} },
      { " U ", 0, {5, BIT3} },
      { " I ", 0, {5, BIT2} },
      { " O ", 0, {5, BIT1} },
      { " P ", 0, {5, BIT0} }
    },
    {
      { " A ", 0, {1, BIT0} },
      { " S ", 0, {1, BIT1} },
      { " D ", 0, {1, BIT2} },
      { " F ", 0, {1, BIT3} },
      { " G ", 0, {1, BIT4} },
      { " H ", 0, {6, BIT4} },
      { " J ", 0, {6, BIT3} },
      { " K ", 0, {6, BIT2} },
      { " L ", 0, {6, BIT1} },
      { "Ent", 0, {6, BIT0} }
    },
    {
      { "Esc", 1, {0, BIT0} },
      { " Z ", 0, {0, BIT1} },
      { " X ", 0, {0, BIT2} },
      { " C ", 0, {0, BIT3} },
      { " V ", 0, {0, BIT4} },
      { " B ", 0, {7, BIT4} },
      { " N ", 0, {7, BIT3} },
      { " M ", 0, {7, BIT2} },
      { "Esc", 1, {7, BIT1} },
      { "Spc", 0, {7, BIT0} }
    }
  };

public:
  PicoOnScreenKeyboard(int32_t x, int32_t y, int32_t w,int32_t h);

  void config(
    std::function<void(uint8_t line,uint8_t key)> press)
  { 
    _press = press; 
  }

  void paintRow(PicoPen *pen, int32_t ox, uint8_t r);
};
