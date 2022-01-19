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

  PicoSelect _tapePlayer;
  PicoOptionText _chooseTapeOp;
  PicoOptionText _ejectTapeOp;
  PicoOption _pauseTapeOp;

  PicoSelect _chooseTape;
  PicoSelect _chooseSnap;

  PicoSelect _reset;
  PicoOptionText _reset48kOp;
  PicoOptionText _reset128kOp;



  PicoWin _message;
  PicoSelect _confirm;
  PicoOptionText _confirmNo;
  PicoOptionText _confirmYes;
  
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

  ZxSpectrumMenu(SdCardFatFsSpi* sdCard, ZxSpectrum *zxSpectrum);
};
