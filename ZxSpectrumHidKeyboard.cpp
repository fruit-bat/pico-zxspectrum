#include "ZxSpectrumHidKeyboard.h"
#include "stdlib.h"

struct ZxSpectrumHidKey {
  unsigned char keycode;
  unsigned char line;
  unsigned char key;
};

#define HID_KEY_LEFTALT 0xe2

ZxSpectrumHidKey KEYS[] = {
  { HID_KEY_SHIFT_LEFT,  0, 0},
  { HID_KEY_SHIFT_RIGHT, 0, 0},
  { HID_KEY_Z,           0, 1},
  { HID_KEY_X,           0, 2},
  { HID_KEY_C,           0, 3},
  { HID_KEY_V,           0, 4},

  { HID_KEY_A,           1, 0},
  { HID_KEY_S,           1, 1},
  { HID_KEY_D,           1, 2},
  { HID_KEY_F,           1, 3},
  { HID_KEY_G,           1, 4},

  { HID_KEY_Q,           2, 0},
  { HID_KEY_W,           2, 1},
  { HID_KEY_E,           2, 2},
  { HID_KEY_R,           2, 3},
  { HID_KEY_T,           2, 4},
  
  { HID_KEY_1,           3, 0},
  { HID_KEY_2,           3, 1},
  { HID_KEY_3,           3, 2},
  { HID_KEY_4,           3, 3},
  { HID_KEY_5,           3, 4},

  { HID_KEY_0,           4, 0},
  { HID_KEY_9,           4, 1},
  { HID_KEY_8,           4, 2},
  { HID_KEY_7,           4, 3},
  { HID_KEY_6,           4, 4},

  { HID_KEY_P,           5, 0},
  { HID_KEY_O,           5, 1},
  { HID_KEY_I,           5, 2},
  { HID_KEY_U,           5, 3},
  { HID_KEY_Y,           5, 4},

  { HID_KEY_ENTER,       6, 0},
  { HID_KEY_L,           6, 1},
  { HID_KEY_K,           6, 2},
  { HID_KEY_J,           6, 3},
  { HID_KEY_H,           6, 4},

  { HID_KEY_SPACE,       7, 0},
  { HID_KEY_LEFTALT,     7, 1}, // Symbol
  { HID_KEY_M,           7, 2},
  { HID_KEY_N,           7, 3},
  { HID_KEY_B,           7, 4},

};

static bool KEYS_SORTED  = false;

static int keys_comparator(const ZxSpectrumHidKey *k1, const ZxSpectrumHidKey *k2) {
  int kk1 = (int)k1->keycode;
  int kk2 = (int)k2->keycode;
  return kk2 - kk1;
}

static int keys_v_comparator(const void *k1, const void *k2) {
  return keys_comparator((ZxSpectrumHidKey *)k1, (ZxSpectrumHidKey *)k2);
}

const static unsigned int KEYS_LEN = sizeof(KEYS) / sizeof(struct ZxSpectrumHidKey);

const static unsigned int KEY_SIZE = sizeof(struct ZxSpectrumHidKey);

static void sort_keys() {
  if (!KEYS_SORTED) {
    qsort(KEYS, KEYS_LEN, KEY_SIZE, keys_v_comparator);
  }
  KEYS_SORTED = true;
}

static ZxSpectrumHidKey* findKey(const unsigned char keycode) {
  if (keycode <= 1) return 0;
  const ZxSpectrumHidKey k0 = {keycode, 0, 0};
  return (ZxSpectrumHidKey *) bsearch(&k0, KEYS, KEYS_LEN, KEY_SIZE, keys_v_comparator);
}

static bool isInReport(hid_keyboard_report_t const *report, const unsigned char keycode) {
  for(unsigned int i = 0; i < 6; i++) {
    if (report->keycode[i] == keycode)  return true;
  }
  return false;
}

ZxSpectrumHidKeyboard::ZxSpectrumHidKeyboard() : _reset1(false), _reset2(false) {
  sort_keys();
}

static unsigned char LOCK_KEYS[] = {
  HID_KEY_CAPS_LOCK
//  HID_KEY_NUM_LOCK
};

void ZxSpectrumHidKeyboard::processHidReport(hid_keyboard_report_t const *report) {
  static hid_keyboard_report_t prev = { 0, 0, {0} };
  static bool lock_flags[sizeof(LOCK_KEYS)] = {false};
  reset();
  if (report->keycode[0] == 1) return;
  const unsigned char m = report->modifier;
  for (int i = 0; i < 8; ++i) {
    if (m & (1 << i)) {
      const ZxSpectrumHidKey *k = findKey(0xE0 | i);
      if (k) press(k->line, k->key);
    }
  }
  for(unsigned int i = 0; i < 6; ++i) {
    const unsigned char hidKeyCode = report->keycode[i];
    
    // F11 & F12 both down together for reset
    bool checkReset = false;
    if (hidKeyCode == HID_KEY_F11) {
      setReset1(true);
      checkReset = !isInReport(&prev, HID_KEY_F11);
    }
    if (hidKeyCode == HID_KEY_F12) {
      setReset2(true); 
      checkReset |= !isInReport(&prev, HID_KEY_F12);
    }
    if (resetPressed() && checkReset) {
      _ZxSpectrum->reset();
    }
      
    // F4 toggle moderate
    if (hidKeyCode == HID_KEY_F4 && !isInReport(&prev, HID_KEY_F4)) {
      _ZxSpectrum->toggleModerate();
    }
        
    const ZxSpectrumHidKey *k = findKey(hidKeyCode);
    if (k) {
      const unsigned char keycode = k->keycode;
      bool isLockKey = false;
      for (unsigned int j = 0; j < sizeof(LOCK_KEYS); ++j) {
        if (keycode == LOCK_KEYS[j] && !isInReport(&prev, keycode)) {
          lock_flags[j] = !lock_flags[j];
          isLockKey = true;
        }
      }
      if (!isLockKey) {
        press(k->line, k->key);
      }
    }
  }
  for (unsigned int j = 0; j < sizeof(LOCK_KEYS); ++j) {
    if (lock_flags[j]) {
      const ZxSpectrumHidKey *k = findKey(LOCK_KEYS[j]);
      if (k) press(k->line, k->key);
    }
  }
  prev = *report;
}
