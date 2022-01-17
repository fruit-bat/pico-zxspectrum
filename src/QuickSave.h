#pragma once

#include "SdCardFatFsSpi.h"
#include "ZxSpectrum.h"

class QuickSave {
  SdCardFatFsSpi* _sdCard;
  char* _name;
public:
  QuickSave(SdCardFatFsSpi* sdCard, const char* name);
  ~QuickSave();
  void save(ZxSpectrum *specy, int slot);
  void load(ZxSpectrum *specy, int slot);
};
