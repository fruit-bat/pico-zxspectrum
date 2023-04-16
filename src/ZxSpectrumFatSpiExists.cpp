#include "ZxSpectrumFatSpiExists.h"
#include "stdio.h"

ZxSpectrumFatSpiExists::ZxSpectrumFatSpiExists(
  SdCardFatFsSpi* sdCard, 
  const char *folder,
  const char *name) :
  _sdCard(sdCard),
  _folder(folder),
  _name(name)
{
}

bool ZxSpectrumFatSpiExists::exists() {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  char name[280];
  sprintf(name, "%s/%s", _folder, _name);
  FILINFO fno;
  FRESULT fr = f_stat(name, &fno);
  return fr == FR_OK; 
}
