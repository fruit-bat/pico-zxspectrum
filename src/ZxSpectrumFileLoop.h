#pragma once

#include "ZxSpectrum.h"

class ZxSpectrumFileLoop {
public:
  ZxSpectrumFileLoop() {}
  virtual ~ZxSpectrumFileLoop() {}
  virtual void next(ZxSpectrum* zxSpectrum);
  virtual void prev(ZxSpectrum* zxSpectrum);
  virtual void curr(ZxSpectrum* zxSpectrum);
  virtual void reload();
};
