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

  PicoWiz _wiz;
  PicoSelect _main;

  PicoOption _tapePlayerOp;
  PicoOption _snapOp;
  PicoOption _freqOp;
  PicoOption _muteOp;
  PicoOptionText _resetOp;
  PicoOptionText _quickSavesOp;

  PicoSelect _tapePlayer;
  PicoOptionText _chooseTapeOp;
  PicoOptionText _ejectTapeOp;
  PicoOption _pauseTapeOp;

  PicoSelect _chooseTape;
  PicoSelect _chooseSnap;

  PicoSelect _reset;
  PicoOptionText _reset48kOp;
  PicoOptionText _reset128kOp;

  std::string _tapeName;
  std::string _snapName;

  PicoWin _message;
  PicoSelect _confirm;
  PicoOptionText _confirmNo;
  PicoOptionText _confirmYes;
  
  PicoSelect _quickSaves;
  PicoOption _quick01Op;
  PicoOption _quick02Op;
  PicoOption _quick03Op;
  PicoOption _quick04Op;
  PicoOption _quick05Op;
  PicoOption _quick06Op;
  PicoOption _quick07Op;
  PicoOption _quick08Op;
  PicoOption _quick09Op;
  PicoOption _quick10Op;
  PicoOption _quick11Op;
  PicoOption _quick12Op;

  PicoSelect _quickSave;
  PicoOptionText _quickSaveLoadOp;
  PicoOptionText _quickSaveToSnapOp;
  int _quickSaveSlot;
  QuickSave *_quickSaveHelper;
  PicoTextField _fileName;

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
  
public:

  ZxSpectrumMenu(SdCardFatFsSpi* sdCard, ZxSpectrum *zxSpectrum, QuickSave *quickSave);
};
