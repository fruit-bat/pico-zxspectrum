#pragma once

#include "SdCardFatFsSpi.h"
#include "ZxSpectrum.h"
#include <string>

class QuickSave {
  SdCardFatFsSpi* _sdCard;
  // TOFO make it clearer this is the folder name
  std::string _name;
public:
  QuickSave(SdCardFatFsSpi* sdCard, const char* name);
  ~QuickSave();
  void save(ZxSpectrum *specy, int slot);
  void load(ZxSpectrum *specy, int slot);
  bool copy(int slot, const char *folder, const char *name);
};
