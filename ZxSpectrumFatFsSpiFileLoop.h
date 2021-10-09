#pragma once

#include "SdCardFatFsSpi.h"
#include "ZxSpectrumFileLoop.h"

class ZxSpectrumFatFsSpiFileLoop : public ZxSpectrumFileLoop {
  SdCardFatFsSpi* _sdCard;
  const char *_folder;
  void reload();
  const char *fext(const char *filename);
  InputStream* _is;
public:
  ZxSpectrumFatFsSpiFileLoop(SdCardFatFsSpi* sdCard, const char *folder);
  ~ZxSpectrumFatFsSpiFileLoop();
  virtual void load(ZxSpectrum* zxSpectrum);
};
