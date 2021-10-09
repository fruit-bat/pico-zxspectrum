#include "ZxSpectrumFileLoop.h"

ZxSpectrumFileLoop::ZxSpectrumFileLoop() :
  _curr(0)
{}

void ZxSpectrumFileLoop::add(ZxSpectrumFile *file) {
  if (_curr) file->link(_curr);
  _curr = file;
}

void ZxSpectrumFileLoop::next(ZxSpectrum* zxSpectrum) {
  if (_curr) {
     _curr = _curr->next();
     load(zxSpectrum);
  }
}
 
void ZxSpectrumFileLoop::prev(ZxSpectrum* zxSpectrum) {
  if (_curr) {
    _curr = _curr->prev();
    load(zxSpectrum);
  }
}

void ZxSpectrumFileLoop::curr(ZxSpectrum* zxSpectrum) {
  if (_curr) {
    load(zxSpectrum);
  }
}
