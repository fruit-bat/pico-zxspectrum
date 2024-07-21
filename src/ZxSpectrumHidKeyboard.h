#pragma once

#include "ZxSpectrumKeyboard.h"
#include "ZxSpectrum.h"
#include "tusb.h"
#include "QuickSave.h"
#include "ZxSpectrumFileLoop.h"


#ifndef HID_KEY_MOD_SAVE
#define HID_KEY_MOD_SAVE KEYBOARD_MODIFIER_LEFTCTRL
#endif
#ifndef HID_KEY_MOD_LOAD
#define HID_KEY_MOD_LOAD KEYBOARD_MODIFIER_LEFTALT
#endif

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
    ZxSpectrumJoystick * zxSpectrumJoystick,
    ZxSpectrumMouse * zxSpectrumMouse
  );
  ZxSpectrumHidKeyboard(
    QuickSave* quickSave, 
    ZxSpectrumJoystick * zxSpectrumJoystick,
    ZxSpectrumMouse * zxSpectrumMouse
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
