#pragma once

#include "ZxSpectrumSnap.h"
#include "SdCardFatFsSpi.h"

class ZxSpectrumFatFsSpiSnapZ80 : public ZxSpectrumSnap {
  char* _name;
  SdCardFatFsSpi* _sdCard;
public:
  ZxSpectrumFatFsSpiSnapZ80(SdCardFatFsSpi* sdCard, const char* name);
  virtual void load(ZxSpectrum *specy);
  ~ZxSpectrumFatFsSpiSnapZ80();
};
