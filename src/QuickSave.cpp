#include "QuickSave.h"
#include "ZxSpectrumMenu.h"

QuickSave::~QuickSave() {
}

void QuickSave::save(ZxSpectrum *specy, int slot) {
  if (_menu) _menu->quickSave(slot);
}

void QuickSave::load(ZxSpectrum *specy, int slot) {
  if (_menu) _menu->quickLoad(slot);
}

