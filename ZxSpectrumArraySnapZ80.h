#pragma once

#include "ZxSpectrumSnap.h"

class ZxSpectrumArraySnapZ80 : public ZxSpectrumSnap {
  const unsigned char* _buf;
  const unsigned int _len;
public:
  ZxSpectrumArraySnapZ80(const unsigned char* buf, const unsigned int len);
  virtual void load(ZxSpectrum *specy);
};
