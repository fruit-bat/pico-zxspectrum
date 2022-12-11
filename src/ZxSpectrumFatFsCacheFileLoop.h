#pragma once
#include "FatFsDirCache.h"
#include "ZxSpectrumFileLoop.h"
#include <functional>

class ZxSpectrumFatFsCacheFileLoop : public ZxSpectrumFileLoop {
  SdCardFatFsSpi* _sdCard;
  FatFsDirCache* _cache;
  uint32_t _i;
  std::function<void(uint32_t i, const char *)> _listener;
  
  static const char *fext(const char *filename);
  void load(ZxSpectrum* zxSpectrum);
  
public:
  ZxSpectrumFatFsCacheFileLoop(SdCardFatFsSpi* sdCard, FatFsDirCache* cache);
  ~ZxSpectrumFatFsCacheFileLoop();
  virtual void reload();
  virtual void next(ZxSpectrum* zxSpectrum);
  virtual void prev(ZxSpectrum* zxSpectrum);
  virtual void curr(ZxSpectrum* zxSpectrum);
  void listener(std::function<void(uint32_t i, const char *)> listener) { _listener = listener; }
};
