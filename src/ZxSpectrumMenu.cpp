#include "ZxSpectrumMenu.h"
#include "PicoPen.h"

#define SAVED_SNAPS_DIR "/sorcerer2/snaps"
#define SAVED_TAPES_DIR "/sorcerer2/tapes"

ZxSpectrumMenu::ZxSpectrumMenu(SdCardFatFsSpi* sdCard, ZxSpectrum *zxSpectrum) :
 PicoWin(0, 0, 80, 30),
  _sdCard(sdCard),
  _zxSpectrum(zxSpectrum),
  _k1('1'), _k2('2'), _k3('3'), _k4('4'), _k5('5'), _k6('6'), 
  _wiz(5, 6, 70, 18),
  _main(0, 0, 70, 6, 3),

  _message(0, 0, 70, 12),
  _confirm(0, 0, 70, 6, 3),
  _confirmNo("No"),
  _confirmYes("Yes")
{
  addChild(&_wiz, true);
  _wiz.push(&_main, [](PicoPen *pen){ pen->printAt(0, 0, false, "Main menu"); }, true);
  _wiz.onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
     return (ascii != 27) || !_wiz.pop(true);
   });
   
  _confirm.addOption(_confirmNo.addQuickKey(&_k1));
  _confirm.addOption(_confirmYes.addQuickKey(&_k2));
  _confirm.enableQuickKeys();

  onPaint([](PicoPen *pen) {
     pen->printAt(0, 0, false, "ZX Spectrum 48K/128K emulator");
     pen->printAt(0, 1, false, "on RP2040 Pico Pi");
     pen->printAt(0, 2, false, "Menu System version 0.1");

     pen->printAt(0, 29, false, "F1 to exit menu");
     pen->printAt(80-14, 29, false, "ESC to go back");
   }); 
}

void ZxSpectrumMenu::showError(std::function<void(PicoPen *pen)> message) {
  _wiz.push(
    &_message, 
    [](PicoPen *pen){ pen->printAt(0, 0, false, "Error:"); },
    true);
  _message.onPaint(message);
}


void ZxSpectrumMenu::confirm(
  std::function<void(PicoPen *pen)> message,
  std::function<void()> yes
) {
  _confirm.focus(0);
  _wiz.push(
    &_confirm, 
    message,
    true);
  _confirmYes.toggle([=]() {
    _wiz.pop(true);
    if (yes) yes();
  });
}

void ZxSpectrumMenu::confirm(
  std::function<void(PicoPen *pen)> message,
  std::function<void()> no,
  std::function<void()> yes
) {
  _confirm.focus(0);
  _wiz.push(
    &_confirm, 
    message,
    true);
  _confirmNo.toggle([=]() {
    _wiz.pop(true);
    if (no) no();
  });
  _confirmYes.toggle([=]() {
    _wiz.pop(true);
    if (yes) yes();
  });
}


