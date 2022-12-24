#pragma once

#include "ZxSpectrum.h"

class ZxSpectrumMenu;

class QuickSave {
private:
  ZxSpectrumMenu* _menu;
public:
  QuickSave() : _menu(0) {}
  void set(ZxSpectrumMenu* menu) { _menu = menu; }
  ~QuickSave();
  void save(ZxSpectrum *specy, int slot);
  void load(ZxSpectrum *specy, int slot);
};
