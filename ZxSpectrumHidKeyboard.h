#pragma once

#include "ZxSpectrumKeyboard.h"
#include "ZxSpectrum.h"
#include "tusb.h"
#include "ZxSpectrumSnapList.h"

class ZxSpectrumHidKeyboard : public ZxSpectrumKeyboard {
  ZxSpectrum *_ZxSpectrum;  
  ZxSpectrumSnapList* _zxSpectrumSnapList;
public:
  ZxSpectrumHidKeyboard(ZxSpectrumSnapList* zxSpectrumSnapList);
  void processHidReport(hid_keyboard_report_t const *report);
  void setZxSpectrum(ZxSpectrum *ZxSpectrum) { _ZxSpectrum = ZxSpectrum; }
};
