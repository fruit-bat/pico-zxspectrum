#pragma once

#include "SdCardFatFsSpi.h"
#include "ZxSpectrumSnapList.h"

class ZxSpectrumDirToSnap {
  SdCardFatFsSpi* _sdCard;
public:
  ZxSpectrumDirToSnap(SdCardFatFsSpi* sdCard);
  
  void addToList(const char *folderName, ZxSpectrumSnapList *list);
};
