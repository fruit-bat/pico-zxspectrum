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
    pen->printAt(0, 0, false, getKeyboardLine(0));
    pen->printAt(1, 2, false, getKeyboardLine(1));
    pen->printAt(2, 4, false, getKeyboardLine(2));
    pen->printAt(0, 6, false, getKeyboardLine(3));
  });  
};

const char* PicoOnScreenKeyboard::getKeyboardLine(uint8_t which) {
 
 int Pos=0;
  for (int i=0; i < 10; i++) {
    Pos+=sprintf(Buff+Pos," %s",Line[which][i].Name);
  }
  //Selected symbol
  if(which==_y) {
    Buff[_x*4]='[';
    Buff[_x*4+4]=']';
  }
  return Buff;
}
