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

class ZxSpectrum;

class ZxSpectrumMenu : public PicoWin {
private:
  SdCardFatFsSpi *_sdCard;
  ZxSpectrum *_zxSpectrum;
  InputStream* _tis;

  PicoQuickKeyAscii _k1;
  PicoQuickKeyAscii _k2;
  PicoQuickKeyAscii _k3;
  PicoQuickKeyAscii _k4;
  PicoQuickKeyAscii _k5;
  PicoQuickKeyAscii _k6;
  PicoQuickKeyAscii _k7;

  PicoWiz _wiz;
  PicoSelect _main;

  PicoOption _tapePlayerOp;
  PicoOption _snapOp;
  PicoOption _freqOp;
  PicoOption _muteOp;
  PicoOption _resetOp;
  PicoOption _joystickOp;
  PicoOptionText _quickSavesOp;

  PicoSelect _snapMgr;
  PicoOptionText _snapLoadOp;
  PicoOptionText _snapRenameOp;
  PicoOptionText _snapDeleteOp;
  PicoOptionText _snapRescanDirOp;

  PicoSelect _tapePlayer;
  PicoOptionText _chooseTapeOp;
  PicoOptionText _ejectTapeOp;
  PicoOption _pauseTapeOp;
  PicoOptionText _tapeRescanDirOp;

  PicoExplorer _chooseTape;
  PicoExplorer _chooseSnap;

  PicoSelect _reset;
  PicoOptionText _reset48kOp;
  PicoOptionText _reset128kOp;

  PicoSelect _joystick;
  PicoOptionText _joystickKemstonOp;
  PicoOptionText _joystickSinclairOp;

  std::string _tmpName;
  std::string _tapeName;
  std::string _snapName;

  PicoWin _devices;
  PicoWin _message;
  PicoSelect _confirm;
  PicoOptionText _confirmNo;
  PicoOptionText _confirmYes;
  
  PicoSelect _quickSaves;
  PicoOption _quickOps[12];

  PicoSelect _quickSave;
  PicoOptionText _quickSaveLoadOp;
  PicoOptionText _quickSaveToSnapOp;
  PicoOptionText _quickSaveClearOp;
  
  PicoSelect _tzxSelect;
  std::function<void(uint32_t option)> _tzxOption;

  int _quickSaveSlot;
  QuickSave *_quickSaveHelper;
  PicoTextField _fileName;
  bool _quickSaveSlotUsed[12];
  
  std::function<void()> _refresh;
  
  void snapName(std::string &fname, const char *name);

  void loadDirAlphabetical(const char* folder, PicoSelect *select);
  
  void ejectTape();
  
  void showError(std::function<void(PicoPen *pen)> message);

  void confirm(
    std::function<void(PicoPen *pen)> message,
    std::function<void()> no,
    std::function<void()> yes
  );
  
  void confirm(
    std::function<void(PicoPen *pen)> message,
    std::function<void()> yes
  );
  
  bool checkExists(const char *file);
  
  bool renameSave(const char *fileo, const char *filen);

  bool deleteSave(const char *folder, const char *file);
  
  void quickSaveToSnap(int i, const char *folder, const char *file);
  
public:
  void showMessage(std::function<void(PicoPen *pen)> message);
  void removeMessage();
  
  void showTzxOptions();
  void clearTzxOptions();
  void addTzxOption(const char *);
  void tzxOption(std::function<void(uint32_t option)> tzxOption) { _tzxOption = tzxOption; }
  void refresh(std::function<void()> refresh) { _refresh = refresh; }
  
  ZxSpectrumMenu(
    FatFsDirCache* snapDirCache,
    FatFsDirCache* tapeDirCache,
    SdCardFatFsSpi* sdCard,
    ZxSpectrum *zxSpectrum,
    QuickSave *quickSave);
};
