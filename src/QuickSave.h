#pragma once

#include "SdCardFatFsSpi.h"
#include "ZxSpectrum.h"
#include <string>
#include <functional>

class QuickSave {
  SdCardFatFsSpi* _sdCard;
  std::string _folder;
  std::function<void(uint32_t i, const char *)> _listener;

  void filename(const char *buf, int slot);
public:
  QuickSave(SdCardFatFsSpi* sdCard, const char* folder);
  ~QuickSave();
  void save(ZxSpectrum *specy, int slot);
  void load(ZxSpectrum *specy, int slot);
  bool copy(int slot, const char *fname);
  bool used(int slot);
  bool clear(int slot);
  void listener(std::function<void(uint32_t i, const char *)> listener) { _listener = listener; }
};
