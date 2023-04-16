#include "ZxSpectrumFatSpiKiosk.h"

ZxSpectrumFatSpiKiosk::ZxSpectrumFatSpiKiosk(SdCardFatFsSpi* sdCard, const char *folder) :
  _exists(sdCard, folder, "kiosk.txt")
{
}

bool ZxSpectrumFatSpiKiosk::isKiosk() {
  return _exists.exists();
}
