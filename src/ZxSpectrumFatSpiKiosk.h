#pragma once

#include "ZxSpectrumKiosk.h"
#include "SdCardFatFsSpi.h"

class ZxSpectrumFatSpiKiosk : public ZxSpectrumKiosk {
private:
  SdCardFatFsSpi* _sdCard;
  const char *_folder;
public:
  ZxSpectrumFatSpiKiosk(SdCardFatFsSpi* sdCard, const char *folder);
  virtual ~ZxSpectrumFatSpiKiosk() {}
  virtual bool isKiosk();
};
