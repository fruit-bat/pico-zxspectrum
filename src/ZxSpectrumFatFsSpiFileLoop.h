#pragma once

#include "SdCardFatFsSpi.h"
#include "ZxSpectrumFileLoop.h"

class ZxSpectrumFatFsSpiFileLoop : public ZxSpectrumFileLoop {
  ZxSpectrumFile *_curr;
  SdCardFatFsSpi* _sdCard;
  const char *_folder;
  const char *fext(const char *filename);
  InputStream* _is;
  ZxSpectrumFile *curr() { return _curr; }
public:
  ZxSpectrumFatFsSpiFileLoop(SdCardFatFsSpi* sdCard, const char *folder);
  ~ZxSpectrumFatFsSpiFileLoop();
  virtual void load(ZxSpectrum* zxSpectrum);
  void reload();
};
