#pragma once

#include "ZxSpectrumKeyboard.h"
#include "ZxSpectrum.h"
#include "tusb.h"
#include "ZxSpectrumSnapList.h"
#include "QuickSave.h"

class ZxSpectrumHidKeyboard : public ZxSpectrumKeyboard {
  ZxSpectrum *_ZxSpectrum;  
  ZxSpectrumSnapList* _zxSpectrumSnapList;
  QuickSave* _quickSave;
public:
  ZxSpectrumHidKeyboard(ZxSpectrumSnapList* zxSpectrumSnapList, QuickSave* quickSave);
  void processHidReport(hid_keyboard_report_t const *report);
  void setZxSpectrum(ZxSpectrum *ZxSpectrum) { _ZxSpectrum = ZxSpectrum; }
};
