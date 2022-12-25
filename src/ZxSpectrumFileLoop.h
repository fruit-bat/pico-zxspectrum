#pragma once

#include "ZxSpectrum.h"

class ZxSpectrumMenu;

class ZxSpectrumFileLoop {
private:
  ZxSpectrumMenu* _menu;
public:
  ZxSpectrumFileLoop() : _menu(0) {}
  void set(ZxSpectrumMenu* menu) { _menu = menu; }
  virtual ~ZxSpectrumFileLoop() {}
  void next(ZxSpectrum* zxSpectrum);
  void prev(ZxSpectrum* zxSpectrum);
  void curr(ZxSpectrum* zxSpectrum);
  void reload();
};
