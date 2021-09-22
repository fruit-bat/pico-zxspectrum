#pragma once
#include "ZxSpectrumSnap.h"

class ZxSpectrumSnapList {
  ZxSpectrumSnap *_curr;
public:
  ZxSpectrumSnapList();
  void add(ZxSpectrumSnap *snap);
  void next(ZxSpectrum *specy);
  void prev(ZxSpectrum *specy);
};
