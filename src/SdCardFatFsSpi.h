#pragma once

#include "sd_card.h"

class SdCardFatFsSpi {
  int _unit;
  sd_card_t *_sdcard;
public:
  SdCardFatFsSpi(int unit);
  bool mount();
  void unmount();
  bool mounted() { return !!_sdcard; }
};
