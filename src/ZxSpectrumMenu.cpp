#include <algorithm>
#include "pico/bootrom.h"
#include "ZxSpectrumMenu.h"
#include "ZxSpectrum.h"
#include "PicoPen.h"
#include "FatFsSpiDirReader.h"
#include "FatFsSpiInputStream.h"
#include "BufferedInputStream.h"
#include "hardware/clocks.h"
#include "ff.h"
#include "ZxSpectrumVoltage.h"
#include "ZxSpectrumAudio.h"
#include "ZxSpectrumAudioVol.h"
#include "ZxSpectrumAudioDriver.h"
#include "PicoCoreVoltage.h"

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
#define SZ_MENU_SEP 1
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

PicoQuickKeyAscii *ZxSpectrumMenu::getQuickKey(uint i) {
  switch(i) {
    case 0: return &_k1;
    case 1: return &_k2;
    case 2: return &_k3;
    case 3: return &_k4;
    case 4: return &_k5;
    case 5: return &_k6;
    case 6: return &_k7;
    case 7: return &_k8;
    case 8: return &_k9;
    default: return &_k1;
  }
}

void ZxSpectrumMenu::setWizLayout(int32_t margin, int32_t cols1, int32_t cols2, int32_t w) {
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
   _mouse.move(0, 0, _wizCols, _mouse.wh());
   _devices.move(0, 2, _wizCols, _devices.wh());
   _tzxSelect.move(0, 0, _wizCols, _tzxSelect.wh());
   _keyboard.move(0 , 0, w, 10),
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
  _kV('v'), _kK('k'), _kS('s'), _kQ('q'),

  _wiz(_wizLeftMargin, 6, _wizCols, _explorerRows * _explorerRowsPerFile),
  _wizUtils(&_wiz, _explorerRowsPerFile, &_k1, &_k2),
  _main(0, 0, _wizCols, 13, _menuRowsPerItem),
  _audioOp("Audio"),
  _tapePlayer(0, 0, _wizCols, 6, _menuRowsPerItem),

  _chooseTapeOp("Tapes"),
  _ejectTapeOp("Eject tape"),
  _chooseTape(&_wizUtils, sdCard, &_pathTapes, 0, 0, _wizCols, _explorerRows, _explorerRowsPerFile),
  _chooseSnap(&_wizUtils, sdCard, &_pathSnaps, 0, 0, _wizCols, _explorerRows, _explorerRowsPerFile),
  
  _audioOptions(0, 0, _wizCols, 6, _menuRowsPerItem),

  _reset(0, 0, _wizCols, 6, _menuRowsPerItem),
  _reset48kOp("Reset 48K ZX Spectrum"),
  _reset128kOp("Reset 128K ZX Spectrum"),
  
  _joystick(0, 0, _wizCols, 6, _menuRowsPerItem),
  _joystickKempstonOp("Kempston"),
  _joystickSinclairLROp("Sinclair L+R"),
  _joystickSinclairRLOp("Sinclair R+L"),

  _mouse(0, 0, _wizCols, 6, _menuRowsPerItem),
  _mouseKempstonMouseOp("Kempston Mouse"),
  _mouseKempstonJoystickOp("Kemptson Joystick"),
  _mouseSinclairJoystickLOp("Sinclair Joystick L"),
  _mouseSinclairJoystickROp("Sinclair Joystick R"),

  _audioOutOptions(0, 0, _wizCols, 6, _menuRowsPerItem),
  _audioNullOp(_zx_spectrum_audio_drivers[0].name),
  _audioPioPwmOp(_zx_spectrum_audio_drivers[1].name),
  _audioPwmOp(_zx_spectrum_audio_drivers[2].name),
  _audioI2sOp(_zx_spectrum_audio_drivers[3].name),
  _audioHdmiOp(_zx_spectrum_audio_drivers[4].name), 

  _settings(0, 0, _wizCols, 6, _menuRowsPerItem),
  _settingsSaveOp("Save"),
  _settingsLoadOp("Load"),

  _system(0, 0, _wizCols, 6, _menuRowsPerItem),
  _systemBootSelOp("Boot select"),

  _volume(0, 0, 16, 16),
  _keyboard(((SZ_FRAME_COLS-44)/2)-2 , 0, 52, 10),

  _devices(0, 2, _wizCols, 2),
 
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
    int32_t jl = (_zxSpectrum->joystick()->isConnectedL() ? 1 : 0) + (_zxSpectrum->mouse()->isConnectedL() ? 1 : 0);
    int32_t jr = (_zxSpectrum->joystick()->isConnectedR() ? 1 : 0) + (_zxSpectrum->mouse()->isConnectedR() ? 1 : 0);
    bool k1 = _zxSpectrum->keyboard1() && _zxSpectrum->keyboard1()->isMounted();
    bool k2 = _zxSpectrum->keyboard2() && _zxSpectrum->keyboard2()->isMounted();
    bool m = _zxSpectrum->mouse() && _zxSpectrum->mouse()->isMounted();
    const float vsys = read_voltage_sensor();
    const float vcore = pico_get_core_voltage();
    const float fmhz = (float)clock_get_hz(clk_sys) / 1000000.0;

    pen->printAtF(0, 0, false, 
        "Clk: %3.1fMhz Vcore:%1.2fv Vsys:%1.2fv",
        fmhz,
        vcore,
        vsys
        );

    pen->printAtF(0, 1, false,"USB: Joystick%s %s%s%s, Keyboard%s %s%s%s, Mice: %s",
       (jl + jr ? "s" : ""),
       (jl ? "L" : ""), (!jl && !jr ? "0" : (!!jl & !!jr ? "&" : "")), (jr ? "R" : ""),
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
  _main.addOption(_audioOp.addQuickKey(&_k5));
  _main.addOption(_resetOp.addQuickKey(&_k6));
  _main.addOption(_joystickOp.addQuickKey(&_k7));
  _main.addOption(_mouseOp.addQuickKey(&_k8));
  _main.addOption(_settingsOp.addQuickKey(&_k9));
  _main.addOption(_keyboardOp.addQuickKey(&_kK));
  _main.addOption(_systemOp.addQuickKey(&_kS));
  _main.addOption(_quickSaveOp.addQuickKey(&_kQ));

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
  _audioOp.toggle([=]() {
    _wiz.push(
      &_audioOptions, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false,"Audio options");
      }, 
      true);
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

  _system.addOption(_systemBootSelOp.addQuickKey(&_k1));
  _system.enableQuickKeys();
  _systemOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s", _wizCol1Width, "System");
  });  
  _systemOp.toggle([=]() {
    _wiz.push(
      &_system, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "System"); }, 
      true);
  });

  _quickSaveOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s", _wizCol1Width, "Quick save");
  });  
  _quickSaveOp.toggle([=]() {
    quickSave(0);
    if (_snapLoadedListener) _snapLoadedListener("QS1");
  });

  _systemBootSelOp.toggle([=]() {
    _wizUtils.confirm(
      [=](PicoPen *pen){
        pen->printAtF(0, 0, false, "Restart Pico into boot loader?");
      },
      [=]() {
        reset_usb_boot(0,0);
      }
    );
  });

  _keyboard.config(
      [this](uint8_t line, uint8_t key) { _zxSpectrum->keyboard1()->virtuallpress(line,key); }
    );
 
  _keyboardOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"(OSK) Keyboard");
  });
  _keyboardOp.toggle([=]() {
    _wiz.push(
      &_keyboard, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "On Screen Keyboard"); }, 
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

  _mouseOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m = "N/A";
    if (_zxSpectrum->mouse()) {
      switch(_zxSpectrum->mouse()->mouseMode()) {
        case ZxSpectrumMouseModeKempstonMouse: m = "Kempston Mouse" ; break;
        case ZxSpectrumMouseModeJoystick: {
          switch(_zxSpectrum->mouse()->mode()) {
            case ZxSpectrumJoystickModeKempston: m = "Kempston Joystick" ; break;
            case ZxSpectrumJoystickModeSinclairLR: m = "Sinclair Joystick L" ; break;
            case ZxSpectrumJoystickModeSinclairRL: m = "Sinclair Joystick R" ; break;
            default: m = "N/A" ; break;
          }
          break;     
        }
        default: m = "N/A" ; break;
      }
    }
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Mouse", _wizCol2Width, m);
  });
  _mouseOp.toggle([=]() {
    _wiz.push(
      &_mouse, 
      [](PicoPen *pen){ pen->printAt(0, 0, false, "Mouse mode"); }, 
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

  // Start of audio settings
  uint audioQuickKeyIndex = 0;
  _audioOptions.addOption(_muteOp.addQuickKey(getQuickKey(audioQuickKeyIndex++)));
  #ifndef BZR_PIN  
  _audioOptions.addOption(_volumeOp.addQuickKey(getQuickKey(audioQuickKeyIndex++)));
  #endif
  _audioOptions.addOption(_audioOutOp.addQuickKey(getQuickKey(audioQuickKeyIndex++)));
  _audioOptions.enableQuickKeys();

  _muteOp.toggle([=]() {
    _zxSpectrum->toggleMute();
    _main.repaint();
  });
  _muteOp.onPaint([=](PicoPen *pen){
    pen->clear();
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Audio", _wizCol2Width, _zxSpectrum->mute() ? "off" : "on");
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
  _audioOutOp.onPaint([=](PicoPen *pen){
    pen->clear();
    const char *m = _zxSpectrum->getAudioDriver()->name;
    pen->printAtF(0, 0, false,"%-*s[ %-*s]", _wizCol1Width, "Audio out", _wizCol2Width, m);
  });
  _audioOutOp.toggle([=]() {
    const char *m = _zxSpectrum->getAudioDriver()->name;
    _wiz.push(
      &_audioOutOptions, 
      [=](PicoPen *pen){ 
        pen->printAtF(0, 0, false, "Audio output (%s)", m); 
      }, 
      true);
  });
// End of audio settings


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

  _joystick.addOption(_joystickKempstonOp.addQuickKey(&_k1));
  _joystick.addOption(_joystickSinclairLROp.addQuickKey(&_k2));
  _joystick.addOption(_joystickSinclairRLOp.addQuickKey(&_k3));
  _joystick.enableQuickKeys();

  _joystickKempstonOp.toggle([=]() {
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

  _mouse.addOption(_mouseKempstonMouseOp.addQuickKey(&_k1));
  _mouse.addOption(_mouseKempstonJoystickOp.addQuickKey(&_k2));
  _mouse.addOption(_mouseSinclairJoystickLOp.addQuickKey(&_k3));
  _mouse.addOption(_mouseSinclairJoystickROp.addQuickKey(&_k4));
  _mouse.enableQuickKeys();

  _mouseKempstonMouseOp.toggle([=]() {
    if (_zxSpectrum->mouse()) {
      _zxSpectrum->mouse()->mouseMode(ZxSpectrumMouseModeKempstonMouse);
    }
    _wiz.pop(true);
  });
  _mouseKempstonJoystickOp.toggle([=]() {
    if (_zxSpectrum->mouse()) {
      _zxSpectrum->mouse()->mouseMode(ZxSpectrumMouseModeJoystick);
      _zxSpectrum->mouse()->mode(ZxSpectrumJoystickModeKempston);
    }
    _wiz.pop(true);
  });
  _mouseSinclairJoystickLOp.toggle([=]() {
    if (_zxSpectrum->mouse()) {
      _zxSpectrum->mouse()->mouseMode(ZxSpectrumMouseModeJoystick);
      _zxSpectrum->mouse()->mode(ZxSpectrumJoystickModeSinclairLR);
    }
    _wiz.pop(true);
  });
  _mouseSinclairJoystickROp.toggle([=]() {
    if (_zxSpectrum->mouse()) {
      _zxSpectrum->mouse()->mouseMode(ZxSpectrumMouseModeJoystick);
      _zxSpectrum->mouse()->mode(ZxSpectrumJoystickModeSinclairRL);
    }
    _wiz.pop(true);
  });
  uint audioOutQuickKeyIndex = 0;
#ifdef PICO_PIO_PWM_AUDIO  
_audioOutOptions.addOption(_audioPioPwmOp.addQuickKey(getQuickKey(audioOutQuickKeyIndex++)));
  _audioPioPwmOp.toggle([=]() {
    _zxSpectrum->setAudioDriver(&_zx_spectrum_audio_drivers[zx_spectrum_audio_driver_pio_pwm_index]);
    _wiz.pop(true);
  });
#endif
#ifdef PICO_PWM_AUDIO  
_audioOutOptions.addOption(_audioPwmOp.addQuickKey(getQuickKey(audioOutQuickKeyIndex++)));
  _audioPwmOp.toggle([=]() {
    _zxSpectrum->setAudioDriver(&_zx_spectrum_audio_drivers[zx_spectrum_audio_driver_pwm_index]);
    _wiz.pop(true);
  });
#endif
#ifdef PICO_AUDIO_I2S  
_audioOutOptions.addOption(_audioI2sOp.addQuickKey(getQuickKey(audioOutQuickKeyIndex++)));
  _audioI2sOp.toggle([=]() {
    _zxSpectrum->setAudioDriver(&_zx_spectrum_audio_drivers[zx_spectrum_audio_driver_i2s_index]);
    _wiz.pop(true);
  });
#endif
#ifdef PICO_HDMI_AUDIO  
_audioOutOptions.addOption(_audioHdmiOp.addQuickKey(getQuickKey(audioOutQuickKeyIndex++)));
  _audioHdmiOp.toggle([=]() {
    _zxSpectrum->setAudioDriver(&_zx_spectrum_audio_drivers[zx_spectrum_audio_driver_hdmi_index]);
    _wiz.pop(true);
  });
#endif
  _audioOutOptions.enableQuickKeys();

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
#ifdef ZX_PLATFORM
     pen->printAtF(0, 0, false, "ZX Spectrum 48K/128K by fruit-bat on %s", ZX_PLATFORM);
#else
     pen->printAt(0, 0, false, "ZX Spectrum 48K/128K by fruit-bat");
#endif
     pen->printAtF(0, 1, false, "Built for %s on %s",
        PICO_MCU,
        __DATE__
      );

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

  _chooseTape.load();
  _chooseSnap.load();
}

void ZxSpectrumMenu::saveSettings() {
  ZxSpectrumSettingValues settings;
  settings.volume = zxSpectrumAudioGetVolume();
  settings.joystickMode = _zxSpectrum->joystick()->mode();
  settings.mouseMode = _zxSpectrum->mouse()->mouseMode();
  settings.mouseJoystickMode = _zxSpectrum->mouse()->mode();
  _zxSpectrumSettings->save(&settings);
  DBG_PRINTF("ZxSpectrumMenu: Saved volume setting '%ld'\n", settings.volume);
  DBG_PRINTF("ZxSpectrumMenu: Saved joystick setting '%d'\n", settings.joystickMode);  
  DBG_PRINTF("ZxSpectrumMenu: Saved mouse setting '%d'\n", settings.mouseMode);  
}

void ZxSpectrumMenu::loadSettings() {
  ZxSpectrumSettingValues settings;
  _zxSpectrumSettings->load(&settings);
  DBG_PRINTF("ZxSpectrumMenu: Loaded volume setting '%ld'\n", settings.volume);
  DBG_PRINTF("ZxSpectrumMenu: Loaded joystick setting '%d'\n", settings.joystickMode);
  DBG_PRINTF("ZxSpectrumMenu: Loaded mouse setting '%d'\n", settings.mouseMode);
  zxSpectrumAudioSetVolume(settings.volume);
  _zxSpectrum->joystick()->mode(settings.joystickMode);
  _zxSpectrum->mouse()->mouseMode(settings.mouseMode);
  _zxSpectrum->mouse()->mode(settings.mouseJoystickMode);
}
