#include <algorithm>

#include "ZxSpectrumMenu.h"
#include "ZxSpectrum.h"
#include "PicoPen.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiInputStream.h"
#include "BufferedInputStream.h"
#include "hardware/clocks.h"
#include "ff.h"

// #define DEBUG_ZX_MENU

#ifdef DEBUG_ZX_MENU
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#ifndef SZ_WIZ_ML
#define SZ_WIZ_ML 3
#endif
#ifndef SZ_WIZ_CW1
#define SZ_WIZ_CW1 16
#endif
#ifndef SZ_WIZ_CW2
#define SZ_WIZ_CW2 40
#endif
#ifndef SZ_FRAME_COLS
#define SZ_FRAME_COLS 80
#endif
#ifndef SZ_FRAME_ROWS
#define SZ_FRAME_ROWS 30
#endif
#ifndef SZ_FRAME_X
#define SZ_FRAME_X 0
#endif
#ifndef SZ_FRAME_Y
#define SZ_FRAME_Y 0
#endif
#ifndef SZ_MENU_SEP
#define SZ_MENU_SEP 2
#endif
#ifndef SZ_FILE_SEP
#define SZ_FILE_SEP 1
#endif
#ifndef SZ_FILE_ROWS
#define SZ_FILE_ROWS 18
#endif

#define SZ_WIZ_COLS (SZ_FRAME_COLS-(2*SZ_WIZ_ML))

static const char *fext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

ZxSpectrumMenu::ZxSpectrumMenu(
    SdCardFatFsSpi* sdCard,
    ZxSpectrum *zxSpectrum
) :
 PicoWin(SZ_FRAME_X, SZ_FRAME_Y, SZ_FRAME_COLS, SZ_FRAME_ROWS),
   _pathZxSpectrum("/zxspectrum"),
   _pathSnaps(&_pathZxSpectrum, "snapshots"),
   _pathTapes(&_pathZxSpectrum, "tapes"),
   _pathQuickSaves(&_pathSnaps, "quicksaves"),
  _sdCard(sdCard),
  _zxSpectrum(zxSpectrum),
  _tis(0),
  _k1('1'), _k2('2'), _k3('3'), _k4('4'), _k5('5'), _k6('6'), _k7('7'),
  _wiz(SZ_WIZ_ML, 6, SZ_WIZ_COLS, SZ_FILE_ROWS * SZ_FILE_SEP),
  _wizUtils(&_wiz, SZ_FILE_SEP, &_k1, &_k2),
  _main(0, 0, SZ_WIZ_COLS, 7, SZ_MENU_SEP),
  
  _tapePlayer(0, 0, SZ_WIZ_COLS, 6, SZ_MENU_SEP),

  _chooseTapeOp("Tapes"),
  _ejectTapeOp("Eject tape"),
  _chooseTape(&_wizUtils, sdCard, &_pathTapes, 0, 0, SZ_WIZ_COLS, SZ_FILE_ROWS, SZ_FILE_SEP),
  _chooseSnap(&_wizUtils, sdCard, &_pathSnaps, 0, 0, SZ_WIZ_COLS, SZ_FILE_ROWS, SZ_FILE_SEP),
  
  _reset(0, 0, SZ_WIZ_COLS, 6, SZ_MENU_SEP),
  _reset48kOp("Reset 48K ZX Spectrum"),
  _reset128kOp("Reset 128K ZX Spectrum"),
  
  _joystick(0, 0, SZ_WIZ_COLS, 6, SZ_MENU_SEP),
  _joystickKemstonOp("Kempston"),
  _joystickSinclairOp("Sinclair"),

  _devices(0, 3, SZ_WIZ_COLS, 1),
 
  _tzxSelect(0, 0, SZ_WIZ_COLS, 6, SZ_MENU_SEP)
{ 
  _pathQuickSaves.createFolders(sdCard);
  _pathTapes.createFolders(sdCard);
  
  _tzxSelect.onToggle(
    [=](PicoOption *option, int32_t i) {
      _wiz.pop(true);
      if (_tzxOption) _tzxOption(i);
    }
  );
  
  addChild(&_devices, false);
  _devices.onPaint([=](PicoPen *pen){
    bool jl = _zxSpectrum->joystick()->isConnectedL();
    bool jr = _zxSpectrum->joystick()->isConnectedR();
    bool k1 = _zxSpectrum->keyboard1() && _zxSpectrum->keyboard1()->isMounted();
    bool k2 = _zxSpectrum->keyboard2() && _zxSpectrum->keyboard2()->isMounted();
    pen->printAtF(0, 0, false,"USB: Joystick%s %s%s%s, Keyboard%s %s%s%s",
       (jl == jr ? "s" : ""),
       (jl ? "L" : ""), (!jl && !jr ? "0" : (jl & jr ? "&" : "")), (jr ? "R" : ""),
       (k1 == k2 ? "s" : ""),
       (k1 ? "1" : ""), (!k1 && !k2 ? "0" : (k1 & k2 ? "&" : "")), (k2 ? "2" : "")
       );
    _devices.repaint();
  });
  
  addChild(&_wiz, true);
  _wiz.push(&_main, [](PicoPen *pen){ pen->printAt(0, 0, false, "Main menu"); }, true);
  _wiz.onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
     return (ascii != 27) || !_wiz.pop(true);
   });
   
  _main.addOption(_snapOp.addQuickKey(&_k1));
  _main.addOption(_tapePlayerOp.addQuickKey(&_k2));
  _main.addOption(_freqOp.addQuickKey(&_k3));
  _main.addOption(_muteOp.addQuickKey(&_k4));
  _main.addOption(_resetOp.addQuickKey(&_k5));
  _main.addOption(_joystickOp.addQuickKey(&_k6));

  _main.enableQuickKeys();
  _snapOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Snapshot", SZ_WIZ_CW2, _snapName.c_str());
  });
  _snapOp.toggle([=]() {
    _wiz.push(
      &_chooseSnap, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"%s  [%s]", "Snaps", "1=DEL 2=REN 3=CPY 4=PST 5=REF");
      },
      true);
  });
  _tapePlayerOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Tape player", SZ_WIZ_CW2, _tapeName.c_str());
  });
  _tapePlayerOp.toggle([=]() {
    _wiz.push(
      &_tapePlayer, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"Tape player [ %-*s]", SZ_WIZ_CW2, _tapeName.c_str());
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
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "CPU Speed", SZ_WIZ_CW2, m);
  });
  _muteOp.toggle([=]() {
    _zxSpectrum->toggleMute();
    _main.repaint();
  });
  _muteOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Audio", SZ_WIZ_CW2, _zxSpectrum->mute() ? "off" : "on");
  });
  _resetOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m;
    switch(_zxSpectrum->type()) {
      case ZxSpectrum48k: m = "48k ZX Spectrum" ; break;
      case ZxSpectrum128k: m = "128k ZX Spectrum" ; break;
      default: m = "" ; break;
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Reset", SZ_WIZ_CW2, m);
  });
  _resetOp.toggle([=]() {
    _wiz.push(
      &_reset, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Reset options"); }, 
      true);
  });
  
  _joystickOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m = "N/A";
    if (_zxSpectrum->joystick()) {
      switch(_zxSpectrum->joystick()->mode()) {
        case ZxSpectrumJoystickModeKempston: m = "Kemptson" ; break;
        case ZxSpectrumJoystickModeSinclair: m = "Sinclair" ; break;
        default: m = "N/A" ; break;
      }
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Joystick", SZ_WIZ_CW2, m);
  });
  _joystickOp.toggle([=]() {
    _wiz.push(
      &_joystick, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Joystick mode"); }, 
      true);
  });

  _chooseSnap.onToggle = [=](FILINFO *finfo, int32_t i, const char* path) {
    snapName(finfo->fname);
    FatFsSpiInputStream *is = new FatFsSpiInputStream(_sdCard, path);
    _zxSpectrum->loadZ80(is);
    delete is;
    if (_snapLoadedListener) _snapLoadedListener(finfo->fname);
  };
   
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

  _joystick.addOption(_joystickKemstonOp.addQuickKey(&_k1));
  _joystick.addOption(_joystickSinclairOp.addQuickKey(&_k2));
  _joystick.enableQuickKeys();

  _joystickKemstonOp.toggle([=]() {
    if (_zxSpectrum->joystick()) {
      _zxSpectrum->joystick()->mode(ZxSpectrumJoystickModeKempston);
    }
    _wiz.pop(true);
  });
  _joystickSinclairOp.toggle([=]() {
    if (_zxSpectrum->joystick()) {
      _zxSpectrum->joystick()->mode(ZxSpectrumJoystickModeSinclair);
    }
    _wiz.pop(true);
  });

  _tapePlayer.addOption(_chooseTapeOp.addQuickKey(&_k1));
  _tapePlayer.addOption(_ejectTapeOp.addQuickKey(&_k2));
  _tapePlayer.addOption(_pauseTapeOp.addQuickKey(&_k3));
  
  _tapePlayer.enableQuickKeys();

  _chooseTapeOp.toggle([=]() {
    _wiz.push(
      &_chooseTape, 
      [](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"%s  [%s]", "Tapes", "1=DEL 2=REN 3=CPY 4=PST 5=REF");
      },
      true);
  });

  _ejectTapeOp.toggle([=]() {
    ejectTape();
    _wiz.repaint();
  });
  
  _pauseTapeOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", SZ_WIZ_CW1, "Motor", SZ_WIZ_CW2, _zxSpectrum->tapePaused() ? "off" : "on");
  });
  _pauseTapeOp.toggle([=]() {
    _zxSpectrum->togglePauseTape();
  });
  
  _chooseTape.onToggle = [=](FILINFO *finfo, int32_t i, const char* path) {
    ejectTape();
    _tapeName = finfo->fname;
    _tis = new FatFsSpiInputStream(_sdCard, path);
    const char *ext = fext(finfo->fname);
    if (0 == strcmp(ext, "tzx") || 0 == strcmp(ext, "TZX")) {
      _zxSpectrum->loadTzx(_tis);
    }
    else {
      _zxSpectrum->loadTap(_tis); 
    }
    _wiz.pop(true);
  };

  onPaint([](PicoPen *pen) {
     pen->printAt(0, 0, false, "ZX Spectrum 48K/128K by fruit-bat");
     pen->printAtF(0, 1, false, "on RP2040 Pico Pi at %3.1fMhz", (float)clock_get_hz(clk_sys) / 1000000.0);
     pen->printAt(0, 2, false, "Menu System version 0.31");

     pen->printAt(0, SZ_FRAME_ROWS-1, false, "F1 to exit menu");
     pen->printAt(SZ_FRAME_COLS-14, SZ_FRAME_ROWS-1, false, "ESC to go back");
   }); 
}

void ZxSpectrumMenu::snapName(const char* name) {
  _snapName = name;
}

void ZxSpectrumMenu::ejectTape() {
  _zxSpectrum->ejectTape();
  _tapeName.clear();
  if (_tis) {
    delete _tis;
    _tis = 0;
  }
}

void ZxSpectrumMenu::showMessage(std::function<void(PicoPen *pen)> message) {
  _wizUtils.showMessage(message);
}

void ZxSpectrumMenu::refresh(std::function<void()> refresh) {
  _chooseSnap.onRefreshDisplay = refresh;
  _chooseTape.onRefreshDisplay = refresh;
}

void ZxSpectrumMenu::removeMessage() {
  _wizUtils.removeMessage();
}

void ZxSpectrumMenu::showTzxOptions() {
  _wiz.push(
    &_tzxSelect, 
    [](PicoPen *pen){ pen->printAt(0, 0, false, "Choose TZX tape option"); },
    true);
}

void ZxSpectrumMenu::clearTzxOptions() {
  _tzxSelect.deleteOptions();
}

void ZxSpectrumMenu::addTzxOption(const char *s) {
  _tzxSelect.addOption(new PicoOptionText(s));
}

bool ZxSpectrumMenu::isZ80(const char* filename) {
    const char *ext = fext(filename);
    return (0 == strcmp(ext, "z80") || 0 == strcmp(ext, "Z80"));
}

void ZxSpectrumMenu::nextSnap(int d) {
  _chooseSnap.next([](FILINFO* finfo) {
    return !(finfo->fattrib & AM_DIR) && isZ80(finfo->fname);
  }, d);
}

void ZxSpectrumMenu::quickSave(int slot) {

  // Make sure we have a folder for the quick-save
  if (!_pathQuickSaves.createFolders(_sdCard)) return;
  
  {
    char name[32];
    snprintf(name, 32, "Slot %d.z80", slot + 1);

    DBG_PRINTF("ZxSpectrumMenu: Quick save file to save name '%s'\n", name);
    
    FatFsFilePath pname(&_pathQuickSaves, name);
    std::string fullpath;
    pname.appendTo(fullpath);
    
    DBG_PRINTF("ZxSpectrumMenu: Quick save file to save path '%s'\n", fullpath.c_str());
    
    FatFsSpiOutputStream os(_sdCard, fullpath.c_str());
    
    _zxSpectrum->saveZ80(&os);
  }
  
  {
    std::string fullpath;
    _pathQuickSaves.appendTo(fullpath);
    _chooseSnap.folderModified(fullpath.c_str());
  }
}

void ZxSpectrumMenu::quickLoad(int slot) {
  char name[32];
  snprintf(name, 32, "Slot %d.z80", slot + 1);

  DBG_PRINTF("ZxSpectrumMenu: Quick save file to load name '%s'\n", name);
  
  FatFsFilePath pname(&_pathQuickSaves, name);
  std::string fullpath;
  pname.appendTo(fullpath);
  
  DBG_PRINTF("ZxSpectrumMenu: Quick save file to load path '%s'\n", fullpath.c_str());  
  
  FatFsSpiInputStream is(_sdCard, fullpath.c_str());
  
  if (is.closed()) {
    DBG_PRINTF("ZxSpectrumMenu: No quick save in slot '%d' to load\n", slot);  
    return;
  }
  
  _zxSpectrum->loadZ80(&is);
  
  snapName(name);
}



