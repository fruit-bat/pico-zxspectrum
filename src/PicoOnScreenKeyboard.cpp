#include "PicoOnScreenKeyboard.h"
#include "PicoPen.h"

PicoOnScreenKeyboard::PicoOnScreenKeyboard(
  int32_t x, int32_t y, int32_t w, int32_t h
) :
  PicoWin(x, y, w, h)
{
  //for (uint32_t i = 0; i < _steps; ++i) _chars.append(" ");
  
  onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
    if (ascii == 131) { // Up
        if (_y<=0) _y=3; else _y--;
        repaint();
        return false;
    } else if (ascii == 130) { // Down
    if (_y>=3) _y=0; else _y++;
        repaint();
        return false;
    } else if (ascii == 129) { // Left
    if (_x<=0) _x=9; else _x--;
        repaint();
        return false;
    } else if (ascii == 128) { // Right
    if (_x>=9) _x=0; else _x++;
        repaint();
        return false;
    } else if (keycode==0x29) { //Keyboard ESC
        return true;
    } else  if (ascii==27) { //Esc aka Button 3 - joystick 
         if (!Line[_y][_x].EscapeChar) {
           const ZxSpectrumKeyContact * k = &Line[_y][_x].contact;
           _press(k->line,k->key);
           return false;
         }
    }
    return true;
  });

  onPaint([=](PicoPen *pen) {
    if (ww() > 42) {
      paintRow(pen, 0, 0);
      paintRow(pen, 1, 1);
      paintRow(pen, 2, 2);
      paintRow(pen, 0, 3);
    }
    else {
      paintRow(pen, 0, 0);
      paintRow(pen, 0, 1);
      paintRow(pen, 0, 2);
      paintRow(pen, 0, 3);
    }
  });
};

void PicoOnScreenKeyboard::paintRow(PicoPen *pen, int32_t ox, uint8_t r) {
  const int32_t y = r << 1;
  for (int i = 0; i < 10; i++) {
    const bool c = r == _y && i == _x;
    if (c) pen->setAttrInverse(true);
    pen->printAt(ox + (i << 2), y, false, Line[r][i].Name);    
    if (c) pen->setAttrInverse(false);
  }
}
