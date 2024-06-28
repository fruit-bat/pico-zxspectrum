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

#define ZX_VERSION_MAJOR 0
#define ZX_VERSION_MINOR 36

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

void ZxSpectrumMenu::setWizLayout(int32_t margin, int32_t cols1, int32_t cols2) {
   _wizCol1Width = cols1;
   _wizCol2Width = cols2;
   _wizLeftMargin = margin;
   _wizCols = ww() - (2 * margin);
   _wiz.move(_wizLeftMargin, _wiz.wy(), _wizCols, _wiz.wh());
   _main.move(0, 0, _wizCols, _main.wh());
   _tapePlayer.move(0, 0, _wizCols, _tapePlayer.wh());
   _chooseTape.move(0, 0, _wizCols, _chooseTape.wh());
   _chooseSnap.move(0, 0, _wizCols, _chooseSnap.wh());
   _reset.move(0, 0, _wizCols, _reset.wh());
   _joystick.move(0, 0, _wizCols, _joystick.wh());
   _devices.move(0, 3, _wizCols, _devices.wh());
   _tzxSelect.move(0, 0, _wizCols, _tzxSelect.wh());
   repaint();
}

ZxSpectrumMenu::ZxSpectrumMenu(
    SdCardFatFsSpi* sdCard,
    ZxSpectrum *zxSpectrum,
    ZxSpectrumSettings *settings
) :
 PicoWin(SZ_FRAME_X, SZ_FRAME_Y, SZ_FRAME_COLS, SZ_FRAME_ROWS),
   _wizCol1Width(SZ_WIZ_CW1),
   _wizCol2Width(SZ_WIZ_CW2),
   _wizLeftMargin(SZ_WIZ_ML),
   _wizCols(SZ_WIZ_COLS),
   _menuRowsPerItem(SZ_MENU_SEP),
   _explorerRowsPerFile(SZ_FILE_SEP),
   _explorerRows(SZ_FILE_ROWS),
   _pathZxSpectrum("/zxspectrum"),
   _pathSnaps(&_pathZxSpectrum, "snapshots"),
   _pathTapes(&_pathZxSpectrum, "tapes"),
   _pathQuickSaves(&_pathSnaps, "quicksaves"),
  _sdCard(sdCard),
  _zxSpectrum(zxSpectrum),
  _zxSpectrumSettings(settings),
  _tis(0),
  _k1('1'), _k2('2'), _k3('3'), _k4('4'), _k5('5'), _k6('6'), _k7('7'), _k8('8'), _k9('9'),
  _wiz(_wizLeftMargin, 6, _wizCols, _explorerRows * _explorerRowsPerFile),
  _wizUtils(&_wiz, _explorerRowsPerFile, &_k1, &_k2),
  _main(0, 0, _wizCols, 9, _menuRowsPerItem),
  
  _tapePlayer(0, 0, _wizCols, 6, _menuRowsPerItem),

  _chooseTapeOp("Tapes"),
  _ejectTapeOp("Eject tape"),
  _chooseTape(&_wizUtils, sdCard, &_pathTapes, 0, 0, _wizCols, _explorerRows, _explorerRowsPerFile),
  _chooseSnap(&_wizUtils, sdCard, &_pathSnaps, 0, 0, _wizCols, _explorerRows, _explorerRowsPerFile),
  
  _reset(0, 0, _wizCols, 6, _menuRowsPerItem),
  _reset48kOp("Reset 48K ZX Spectrum"),
  _reset128kOp("Reset 128K ZX Spectrum"),
  
  _joystick(0, 0, _wizCols, 6, _menuRowsPerItem),
  _joystickKemstonOp("Kempston"),
  _joystickSinclairLROp("Sinclair L+R"),
  _joystickSinclairRLOp("Sinclair R+L"),

  _settings(0, 0, _wizCols, 6, _menuRowsPerItem),
  _settingsSaveOp("Save"),
  _settingsLoadOp("Load"),

  _volume(0, 0, 16, 16),

  _devices(0, 3, _wizCols, 1),
 
  _tzxSelect(0, 0, _wizCols, 6, _menuRowsPerItem)
{ 
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
    bool m = _zxSpectrum->mouse() && _zxSpectrum->mouse()->isMounted();
    pen->printAtF(0, 0, false,"USB: Joystick%s %s%s%s, Keyboard%s %s%s%s, Mice: %s",
       (jl == jr ? "s" : ""),
       (jl ? "L" : ""), (!jl && !jr ? "0" : (jl & jr ? "&" : "")), (jr ? "R" : ""),
       (k1 == k2 ? "s" : ""),
       (k1 ? "1" : ""), (!k1 && !k2 ? "0" : (k1 & k2 ? "&" : "")), (k2 ? "2" : ""),
       (m ? "1" : "0")
       );
    _devices.repaint();
  });
  
  addChild(&_wiz, true);
  _wiz.push(&_main, [](PicoPen *pen){ pen->printAt(0, 0, false, "Main menu"); }, true);
  _wiz.onKeydown([=](uint8_t keycode, uint8_t modifiers, uint8_t ascii) {
     // Escape or Left for back
     return (ascii != 27 && ascii != 129) || !_wiz.pop(true);
   });
   
  _main.addOption(_snapOp.addQuickKey(&_k1));
  _main.addOption(_tapePlayerOp.addQuickKey(&_k2));
  _main.addOption(_freqOp.addQuickKey(&_k3));
  _main.addOption(_intSourceOp.addQuickKey(&_k4));
  _main.addOption(_muteOp.addQuickKey(&_k5));
  _main.addOption(_resetOp.addQuickKey(&_k6));
  _main.addOption(_joystickOp.addQuickKey(&_k7));
  _main.addOption(_settingsOp.addQuickKey(&_k8));
#ifndef BZR_PIN  
  _main.addOption(_volumeOp.addQuickKey(&_k9));
#endif
  _main.enableQuickKeys();
  _snapOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Snapshot", _wizCol2Width, _snapName.c_str());
  });
  _snapOp.toggle([=]() {
    _wiz.push(
      &_chooseSnap, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false, "%s  [%s]", "Snaps", 
          SZ_FRAME_COLS < 50 ? "1=DE 2=RN 3=CP 4=PA 5=RF 6=SA" : "1=DEL 2=REN 3=CPY 4=PST 5=REF 6=SAV");
      },
      true);
  });
  _tapePlayerOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Tape player", _wizCol2Width, _tapeName.c_str());
  });
  _tapePlayerOp.toggle([=]() {
    _wiz.push(
      &_tapePlayer, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"Tape player [ %-*s]", _wizCol2Width, _tapeName.c_str());
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
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "CPU Speed", _wizCol2Width, m);
  });
  _intSourceOp.toggle([=]() {
    _zxSpectrum->toggleIntSource();
    _main.repaint();
  });
  _intSourceOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m;
    switch(_zxSpectrum->intSource()) {
      case SyncToCpu: m = "Count CPU cycles" ; break;
      case SyncToDisplay: m = "Sync to display" ; break;
      default: m = "Unknown" ; break;
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "CPU Interrupt", _wizCol2Width, m);
  });
  _muteOp.toggle([=]() {
    _zxSpectrum->toggleMute();
    _main.repaint();
  });
  _muteOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Audio", _wizCol2Width, _zxSpectrum->mute() ? "off" : "on");
  });
  _resetOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m;
    switch(_zxSpectrum->type()) {
      case ZxSpectrum48k: m = "48k ZX Spectrum" ; break;
      case ZxSpectrum128k: m = "128k ZX Spectrum" ; break;
      default: m = "" ; break;
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Reset", _wizCol2Width, m);
  });
  _resetOp.toggle([=]() {
    _wiz.push(
      &_reset, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Reset options"); }, 
      true);
  });

  _settings.addOption(_settingsSaveOp.addQuickKey(&_k1));
  _settings.addOption(_settingsLoadOp.addQuickKey(&_k2));
  _settings.enableQuickKeys();
  _settingsSaveOp.toggle([=]() {
    saveSettings();
    _wiz.pop(true);
  });
  _settingsLoadOp.toggle([=]() {
    loadSettings();
    _wiz.pop(true);
  });
  _settingsOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s", _wizCol1Width, "Settings");
  });  
  _settingsOp.toggle([=]() {
    _wiz.push(
      &_settings, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Settings"); }, 
      true);
  });

  _volume.config(
    [](uint32_t v) { zxSpectrumAudioSetVolume(v); },
    []() { return zxSpectrumAudioGetVolume(); }
  );
  _volumeOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Volume", _wizCol2Width, _volume.getChars());
  });
  _volumeOp.toggle([=]() {
    _wiz.push(
      &_volume, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Volume control"); }, 
      true);
  });

  _joystickOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m = "N/A";
    if (_zxSpectrum->joystick()) {
      switch(_zxSpectrum->joystick()->mode()) {
        case ZxSpectrumJoystickModeKempston: m = "Kempston" ; break;
        case ZxSpectrumJoystickModeSinclairLR: m = "Sinclair L+R" ; break;
        case ZxSpectrumJoystickModeSinclairRL: m = "Sinclair R+L" ; break;
        default: m = "N/A" ; break;
      }
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Joystick", _wizCol2Width, m);
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
  _chooseSnap.onWizSaveFile = [=](FatFsSpiOutputStream *os) {
    return _zxSpectrum->saveZ80(os);
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
  _joystick.addOption(_joystickSinclairLROp.addQuickKey(&_k2));
  _joystick.addOption(_joystickSinclairRLOp.addQuickKey(&_k3));
  _joystick.enableQuickKeys();

  _joystickKemstonOp.toggle([=]() {
    if (_zxSpectrum->joystick()) {
      _zxSpectrum->joystick()->mode(ZxSpectrumJoystickModeKempston);
    }
    _wiz.pop(true);
  });
  _joystickSinclairLROp.toggle([=]() {
    if (_zxSpectrum->joystick()) {
      _zxSpectrum->joystick()->mode(ZxSpectrumJoystickModeSinclairLR);
    }
    _wiz.pop(true);
  });
  _joystickSinclairRLOp.toggle([=]() {
    if (_zxSpectrum->joystick()) {
      _zxSpectrum->joystick()->mode(ZxSpectrumJoystickModeSinclairRL);
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
        pen->printAtF(0, 0, false,"%s  [%s]", "Tapes", 
          SZ_FRAME_COLS < 50 ? "1=DE 2=RN 3=CP 4=PA 5=RF" : "1=DEL 2=REN 3=CPY 4=PST 5=REF"
        );
      },
      true);
  });

  _ejectTapeOp.toggle([=]() {
    ejectTape();
    _wiz.repaint();
  });
  
  _pauseTapeOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Motor", _wizCol2Width, _zxSpectrum->tapePaused() ? "off" : "on");
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
#ifdef MURMULATOR    
     pen->printAt(0, 0, false, "ZX Spectrum 48K/128K by fruit-bat on MURMULATOR");
#else
     pen->printAt(0, 0, false, "ZX Spectrum 48K/128K by fruit-bat");
#endif
     pen->printAtF(0, 1, false, "on RP2040 Pico Pi at %3.1fMhz", (float)clock_get_hz(clk_sys) / 1000000.0);
     pen->printAtF(0, 2, false, "Menu System version %d.%d", ZX_VERSION_MAJOR, ZX_VERSION_MINOR);

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

void ZxSpectrumMenu::initialise() {
  loadSettings();
  
  _pathQuickSaves.createFolders(_sdCard);
  _pathTapes.createFolders(_sdCard);
}

void ZxSpectrumMenu::saveSettings() {
  ZxSpectrumSettingValues settings;
  settings.volume = zxSpectrumAudioGetVolume();
  settings.joystickMode = _zxSpectrum->joystick()->mode();
  _zxSpectrumSettings->save(&settings);
  DBG_PRINTF("ZxSpectrumMenu: Saved volume setting '%ld'\n", settings.volume);
  DBG_PRINTF("ZxSpectrumMenu: Saved joystick setting '%d'\n", settings.joystickMode);  
}

void ZxSpectrumMenu::loadSettings() {
  ZxSpectrumSettingValues settings;
  _zxSpectrumSettings->load(&settings);
  DBG_PRINTF("ZxSpectrumMenu: Loaded volume setting '%ld'\n", settings.volume);
  DBG_PRINTF("ZxSpectrumMenu: Loaded joystick setting '%d'\n", settings.joystickMode);
  zxSpectrumAudioSetVolume(settings.volume);
  _zxSpectrum->joystick()->mode(settings.joystickMode);
}
