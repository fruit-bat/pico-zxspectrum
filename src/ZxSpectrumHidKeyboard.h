#pragma once

#include "ZxSpectrumKeyboard.h"
#include "ZxSpectrum.h"
#include "tusb.h"
#include "QuickSave.h"
#include "ZxSpectrumFileLoop.h"

class ZxSpectrumHidKeyboard : public ZxSpectrumKeyboard {
  ZxSpectrum *_ZxSpectrum;  
  ZxSpectrumFileLoop* _zxSpectrumSnapList;
  QuickSave* _quickSave;
  bool _kiosk;
  uint8_t _mounted;
public:
  ZxSpectrumHidKeyboard(
    ZxSpectrumFileLoop* zxSpectrumSnapList,
    QuickSave* quickSave, 
    ZxSpectrumJoystick * zxSpectrumJoystick
  );
  ZxSpectrumHidKeyboard(
    QuickSave* quickSave, 
    ZxSpectrumJoystick * zxSpectrumJoystick
  );
  int __not_in_flash_func(processHidReport)(
    hid_keyboard_report_t const *report, 
    hid_keyboard_report_t const *prev_report
  );
  void setZxSpectrum(ZxSpectrum *ZxSpectrum) { _ZxSpectrum = ZxSpectrum; }
  void setKiosk(bool kiosk) { _kiosk = kiosk; }
  virtual bool isMounted() { return _mounted > 0; }
  void mount() { _mounted++; }
  void unmount() { _mounted--; }
};
