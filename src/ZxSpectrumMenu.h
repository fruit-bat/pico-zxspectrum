#pragma once

#include "PicoWin.h"
#include "PicoSelect.h"
#include "PicoOptionText.h"
#include "PicoQuickKeyAscii.h"
#include "PicoWiz.h" 
#include "SdCardFatFsSpi.h"
#include "PicoTextField.h"
#include "InputStream.h"
#include <functional>
#include "QuickSave.h"
#include "PicoExplorer.h"
#include "FatFsFilePath.h"

#include "PicoWizUtils.h"
#include "PicoWizExplorer.h"
#include "PicoSlider.h"
#include "PicoOnScreenKeyboard.h"
#include "ZxSpectrumSettings.h"

class ZxSpectrum;

class ZxSpectrumMenu : public PicoWin {
private:
  int32_t _wizCol1Width;
  int32_t _wizCol2Width;
  int32_t _wizLeftMargin;
  int32_t _wizCols;
  int32_t _menuRowsPerItem;
  int32_t _explorerRowsPerFile;
  int32_t _explorerRows;

  FatFsFilePath _pathZxSpectrum;
  FatFsFilePath _pathSnaps;
  FatFsFilePath _pathTapes;
  FatFsFilePath _pathQuickSaves;
  SdCardFatFsSpi *_sdCard;
  ZxSpectrum *_zxSpectrum;
  ZxSpectrumSettings *_zxSpectrumSettings;
  InputStream* _tis;

  PicoQuickKeyAscii _k1;
  PicoQuickKeyAscii _k2;
  PicoQuickKeyAscii _k3;
  PicoQuickKeyAscii _k4;
  PicoQuickKeyAscii _k5;
  PicoQuickKeyAscii _k6;
  PicoQuickKeyAscii _k7;
  PicoQuickKeyAscii _k8;
  PicoQuickKeyAscii _k9;
  PicoQuickKeyAscii _kV;
  PicoQuickKeyAscii _kK;
  PicoQuickKeyAscii _kS;
  PicoQuickKeyAscii _kQ;

  PicoWiz _wiz;
  PicoWizUtils _wizUtils;
  PicoSelect _main;

  PicoOption _tapePlayerOp;
  PicoOption _snapOp;
  PicoOption _freqOp;
  PicoOption _intSourceOp;
  PicoOption _muteOp;
  PicoOption _resetOp;
  PicoOption _joystickOp;
  PicoOption _mouseOp;
  PicoOption _audioOp;
  PicoOption _settingsOp;
  PicoOption _volumeOp;
  PicoOption _keyboardOp;
  PicoOption _systemOp;
  PicoOption _quickSaveOp;

  PicoSelect _tapePlayer;
  PicoOptionText _chooseTapeOp;
  PicoOptionText _ejectTapeOp;
  PicoOption _pauseTapeOp;

  PicoWizExplorer _chooseTape;
  PicoWizExplorer _chooseSnap;

  PicoSelect _reset;
  PicoOptionText _reset48kOp;
  PicoOptionText _reset128kOp;

  PicoSelect _joystick;
  PicoOptionText _joystickKempstonOp;
  PicoOptionText _joystickSinclairLROp;
  PicoOptionText _joystickSinclairRLOp;

  PicoSelect _mouse;
  PicoOptionText _mouseKempstonMouseOp;
  PicoOptionText _mouseKempstonJoystickOp;
  PicoOptionText _mouseSinclairJoystickLOp;
  PicoOptionText _mouseSinclairJoystickROp;

  PicoSelect _audio;
  PicoOptionText _audioNullOp;
  PicoOptionText _audioPioPwmOp;
  PicoOptionText _audioPwmOp;
  PicoOptionText _audioI2sOp;
  PicoOptionText _audioHdmiOp;

  PicoSelect _settings;
  PicoOptionText _settingsSaveOp;
  PicoOptionText _settingsLoadOp;
  PicoOptionText _settingsAudioOp; // TODO Find a better home

  PicoSelect _system;
  PicoOptionText _systemBootSelOp;

  PicoSlider _volume;
  PicoOnScreenKeyboard _keyboard;

  std::string _tapeName;
  std::string _snapName;

  PicoWin _devices;
  PicoSelect _tzxSelect;
  std::function<void(uint32_t option)> _tzxOption;
  std::function<void(const char *name)> _snapLoadedListener;
   
  void ejectTape();
  
  static bool isZ80(const char* filename);
  
  PicoQuickKeyAscii *getQuickKey(uint i);

public:
  void showMessage(std::function<void(PicoPen *pen)> message);
  void removeMessage();
  
  void showTzxOptions();
  void clearTzxOptions();
  void addTzxOption(const char *);
  void tzxOption(std::function<void(uint32_t option)> tzxOption) { _tzxOption = tzxOption; }
  void refresh(std::function<void()> refresh);
  void snapName(const char* name);
  void snapLoaded(std::function<void(const char *name)> listener) { _snapLoadedListener = listener; }
  void nextSnap(int d);
  void quickSave(int slot);
  void quickLoad(int slot);
  void setWizLayout(int32_t margin, int32_t cols1, int32_t cols2, int32_t w);
  void initialise();
  void saveSettings();
  void loadSettings();
  
  ZxSpectrumMenu(
    SdCardFatFsSpi* sdCard,
    ZxSpectrum *zxSpectrum,
    ZxSpectrumSettings *settings
  );
};
