#include "ZxSpectrumFileLoop.h"
#include "ZxSpectrumMenu.h"

void ZxSpectrumFileLoop::next(ZxSpectrum* zxSpectrum) {
  if (_menu) _menu->nextSnap(1);
}

void ZxSpectrumFileLoop::prev(ZxSpectrum* zxSpectrum) {
  if (_menu) _menu->nextSnap(-1);
}

void ZxSpectrumFileLoop::curr(ZxSpectrum* zxSpectrum) {
  if (_menu) _menu->nextSnap(0);
}

void ZxSpectrumFileLoop::reload() {
  
}
