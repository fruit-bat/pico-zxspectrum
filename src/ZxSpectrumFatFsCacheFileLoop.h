#pragma once
#include "FatFsDirCache.h"
#include "ZxSpectrumFileLoop.h"

class ZxSpectrumFatFsCacheFileLoop : public ZxSpectrumFileLoop {
  SdCardFatFsSpi* _sdCard;
  FatFsDirCache* _cache;
  uint32_t _i;

  const char *fext(const char *filename);
  void load(ZxSpectrum* zxSpectrum);
  
public:
  ZxSpectrumFatFsCacheFileLoop(SdCardFatFsSpi* sdCard, FatFsDirCache* cache);
  ~ZxSpectrumFatFsCacheFileLoop();
  virtual void reload();
  virtual void next(ZxSpectrum* zxSpectrum);
  virtual void prev(ZxSpectrum* zxSpectrum);
  virtual void curr(ZxSpectrum* zxSpectrum);  
};
