#pragma once

#include "SdCardFatFsSpi.h"

class ZxSpectrumFatSpiExists {
private:
  SdCardFatFsSpi* _sdCard;
  const char *_folder;
  const char *_name;
public:
  ZxSpectrumFatSpiExists(
    SdCardFatFsSpi* sdCard,
     const char *folder,
     const char *name);

  virtual ~ZxSpectrumFatSpiExists() {}
  virtual bool exists();
};
