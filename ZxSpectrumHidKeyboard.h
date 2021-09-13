#pragma once

#include "ZxSpectrumKeyboard.h"
#include "ZxSpectrum.h"
#include "tusb.h"

class ZxSpectrumHidKeyboard : public ZxSpectrumKeyboard {
  ZxSpectrum *_ZxSpectrum;  
  bool _reset1;
  bool _reset2;  
public:
  ZxSpectrumHidKeyboard();
  void processHidReport(hid_keyboard_report_t const *report);
  void setZxSpectrum(ZxSpectrum *ZxSpectrum) { _ZxSpectrum = ZxSpectrum; }
  
  void setReset1(bool pressed) {
    _reset1 = pressed;
  }

  void setReset2(bool pressed) {
    _reset2 = pressed;
  }
  
  bool resetPressed() {
    return _reset1 && _reset2;
  }  
};
