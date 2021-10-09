#pragma once

#include "ZxSpectrumFile.h"
#include "ZxSpectrum.h"

class ZxSpectrumFileLoop {
  ZxSpectrumFile *_curr;
protected:
  void add(ZxSpectrumFile *file);
  virtual void load(ZxSpectrum* zxSpectrum) {}
  ZxSpectrumFile *curr() { return _curr; }
public:
  ZxSpectrumFileLoop();
  void next(ZxSpectrum* zxSpectrum);
  void prev(ZxSpectrum* zxSpectrum);
  void curr(ZxSpectrum* zxSpectrum);
};
