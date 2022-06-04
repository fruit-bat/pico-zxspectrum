#include "ZxSpectrumFatSpiKiosk.h"

ZxSpectrumFatSpiKiosk::ZxSpectrumFatSpiKiosk(SdCardFatFsSpi* sdCard, const char *folder) :
  _sdCard(sdCard),
  _folder(folder)
{
}

bool ZxSpectrumFatSpiKiosk::isKiosk() {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  char name[280];
  sprintf(name, "%s/%s", _folder, "kiosk.txt");
  FILINFO fno;
  FRESULT fr = f_stat(name, &fno);
  return fr == FR_OK; 
}
