#include "ZxSpectrumMenu.h"
#include "ZxSpectrum.h"
#include "PicoPen.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiInputStream.h"
#include "BufferedInputStream.h"

#define SAVED_SNAPS_DIR "/zxspectrum/snapshots"
#define SAVED_TAPES_DIR "/zxspectrum/tapes"

ZxSpectrumMenu::ZxSpectrumMenu(SdCardFatFsSpi* sdCard, ZxSpectrum *zxSpectrum) :
 PicoWin(0, 0, 80, 30),
  _sdCard(sdCard),
  _zxSpectrum(zxSpectrum),
  _k1('1'), _k2('2'), _k3('3'), _k4('4'), _k5('5'), _k6('6'), 
  _wiz(5, 6, 70, 18),
  _main(0, 0, 70, 6, 3),
  _resetOp("Reset"),

  _tapePlayer(0, 0, 70, 6, 3),

  _chooseTapeOp("Choose a tape"),
  _ejectTapeOp("Eject tape"),
  _chooseTape(0, 0, 70, 9, 2),
  _chooseSnap(0, 0, 70, 9, 2),
  
  _reset(0, 0, 70, 6, 3),
  _reset48kOp("Reset 48K ZX Spectrum"),
  _reset128kOp("Reset 128K ZX Spectrum"),

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

  _main.addOption(_snapOp.addQuickKey(&_k1));
  _main.addOption(_tapePlayerOp.addQuickKey(&_k2));
  _main.addOption(_freqOp.addQuickKey(&_k3));
  _main.addOption(_muteOp.addQuickKey(&_k4));
  _main.addOption(_resetOp.addQuickKey(&_k5));
  _main.enableQuickKeys();
  _snapOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-16s[ %-40s]", "SNAP Loader", _snapName.c_str());
  });
  _snapOp.toggle([=]() {
    _wiz.push(
      &_chooseSnap, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Choose snapshot:"); },
      true);
    FatFsSpiDirReader dirReader(_sdCard, SAVED_SNAPS_DIR);
    _chooseSnap.deleteOptions();
    dirReader.foreach([=](const FILINFO* info) {
      _chooseSnap.addOption(new PicoOptionText(info->fname));
    });
  });
  _tapePlayerOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-16s[ %-40s]", "Tape player", _tapeName.c_str());
  });
  _tapePlayerOp.toggle([=]() {
    _wiz.push(
      &_tapePlayer, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"Tape player [ %-40s]", _tapeName.c_str());
      }, 
      true);
  });
  _freqOp.toggle([=]() {
    _zxSpectrum->toggleModerate();
    _main.repaint();
  });
  _freqOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m;
    switch(_zxSpectrum->moderate()) {
      case 9: m = "3.5 Mhz" ; break;
      case 8: m = "4.0 Mhz" ; break;
      case 0: m = "Unmoderated" ; break;
      default: m = "Unknown" ; break;
    }
    pen->printAtF(0, 0, false,"%-16s[ %-12s]", "CPU Speed", m);
  });
  _muteOp.toggle([=]() {
    _zxSpectrum->toggleMute();
    _main.repaint();
  });
  _muteOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-16s[ %-12s]", "Audio", _zxSpectrum->mute() ? "off" : "on");
  });
  _resetOp.toggle([=]() {
    _wiz.push(
      &_reset, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Reset options"); }, 
      true);
  });
  
  _reset.addOption(_reset48kOp.addQuickKey(&_k1));
  _reset.addOption(_reset128kOp.addQuickKey(&_k2));
  _reset.enableQuickKeys();

  _reset48kOp.toggle([=]() {
    _snapName.clear();
    _zxSpectrum->reset(ZxSpectrum48k);
    _wiz.pop(true);
  });
  _reset128kOp.toggle([=]() {
    _snapName.clear();
    _zxSpectrum->reset(ZxSpectrum128k);
    _wiz.pop(true);
  });

  _tapePlayer.addOption(_chooseTapeOp.addQuickKey(&_k1));
  _tapePlayer.addOption(_ejectTapeOp.addQuickKey(&_k2));
  _tapePlayer.addOption(_pauseTapeOp.addQuickKey(&_k3));
  _tapePlayer.enableQuickKeys();

  _chooseTapeOp.toggle([=]() {
    _wiz.push(
      &_chooseTape, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Choose tape:"); },
      true);
    FatFsSpiDirReader dirReader(_sdCard, SAVED_TAPES_DIR);
    _chooseTape.deleteOptions();
    dirReader.foreach([=](const FILINFO* info) {
      _chooseTape.addOption(new PicoOptionText(info->fname));
    });
  });

  _ejectTapeOp.toggle([=]() {
    ejectTape();
    _tapePlayer.repaint();
  });
  
  _pauseTapeOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-16s[ %-12s]", "Motor", _zxSpectrum->tapePaused() ? "off" : "on");
  });
  _pauseTapeOp.toggle([=]() {
    _zxSpectrum->togglePauseTape();
  });
  
  _chooseSnap.onToggle([=](PicoOption *option) {
    PicoOptionText *textOption = (PicoOptionText *)option;
    std::string fname(SAVED_SNAPS_DIR);
    fname.append("/");
    fname.append(textOption->text());
    _snapName = textOption->text();
    FatFsSpiInputStream *is = new FatFsSpiInputStream(_sdCard, fname.c_str());
    _zxSpectrum->loadZ80(is);
    delete is;
    _wiz.pop(true);
  });
  
  _chooseTape.onToggle([=](PicoOption *option) {
    PicoOptionText *textOption = (PicoOptionText *)option;
    ejectTape();
    std::string fname(SAVED_TAPES_DIR);
    fname.append("/");
    fname.append(textOption->text());
    _tapeName = textOption->text();
    _tis = new FatFsSpiInputStream(_sdCard, fname.c_str());
    _zxSpectrum->loadTap(_tis);
    _wiz.pop(true);
  });
  
  onPaint([](PicoPen *pen) {
     pen->printAt(0, 0, false, "ZX Spectrum 48K/128K emulator");
     pen->printAt(0, 1, false, "on RP2040 Pico Pi");
     pen->printAt(0, 2, false, "Menu System version 0.1");

     pen->printAt(0, 29, false, "F1 to exit menu");
     pen->printAt(80-14, 29, false, "ESC to go back");
   }); 
}

void ZxSpectrumMenu::ejectTape() {
  // TODO ask the spectrum for the input stream
  if (_tis) {
    _zxSpectrum->loadTap(0);
    _tapeName.clear();
    delete _tis;
    _tis = 0;
  }
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


