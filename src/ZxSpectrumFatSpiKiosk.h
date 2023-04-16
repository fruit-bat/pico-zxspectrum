#pragma once

#include "ZxSpectrumKiosk.h"
#include "ZxSpectrumFatSpiExists.h"

class ZxSpectrumFatSpiKiosk : public ZxSpectrumKiosk {
private:
  ZxSpectrumFatSpiExists _exists;
public:
  ZxSpectrumFatSpiKiosk(SdCardFatFsSpi* sdCard, const char *folder);
  virtual ~ZxSpectrumFatSpiKiosk() {}
  virtual bool isKiosk();
};
