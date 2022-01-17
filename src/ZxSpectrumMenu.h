#pragma once

#include "PicoWin.h"
#include "PicoSelect.h"
#include "PicoOptionText.h"
#include "PicoQuickKeyAscii.h"
#include "PicoWiz.h" 
#include "SdCardFatFsSpi.h"
#include "PicoTextField.h"
#include <functional>

class ZxSpectrum;

class ZxSpectrumMenu : public PicoWin {
private:
  SdCardFatFsSpi *_sdCard;
  ZxSpectrum *_zxSpectrum;

  PicoQuickKeyAscii _k1;
  PicoQuickKeyAscii _k2;
  PicoQuickKeyAscii _k3;
  PicoQuickKeyAscii _k4;
  PicoQuickKeyAscii _k5;
  PicoQuickKeyAscii _k6;

  PicoWiz _wiz;
  PicoSelect _main;

  PicoOption _freqOp;
  PicoOption _muteOp;

  PicoWin _message;
  PicoSelect _confirm;
  PicoOptionText _confirmNo;
  PicoOptionText _confirmYes;
  
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
