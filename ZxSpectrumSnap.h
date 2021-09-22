#pragma once
#include "ZxSpectrum.h"

class ZxSpectrumSnap {
  ZxSpectrumSnap *_p;
  ZxSpectrumSnap *_n;
public:
  ZxSpectrumSnap() : _p(this), _n(this) {}
  
  void link(ZxSpectrumSnap *c) {
      // Link this after c
      _n = c->_n;
      _n->_p = this;
      c->_n = this;
      _p = c;
  }
  
  ZxSpectrumSnap* prev() { return _p; }
  ZxSpectrumSnap* next() { return _n; }
  
  virtual void load(ZxSpectrum *specy);
};
