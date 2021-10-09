#include "ZxSpectrumHidKeyboard.h"
#include "stdlib.h"
#include <pico/printf.h>

struct ZxSpectrumHidKey {
  unsigned char keycode;
  unsigned char line;
  unsigned char key;
};


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

ZxSpectrumHidKeyboard::ZxSpectrumHidKeyboard(ZxSpectrumFileLoop *zxSpectrumSnapList, ZxSpectrumFileLoop* zxSpectrumTapeList, QuickSave* quickSave) :
  _zxSpectrumSnapList(zxSpectrumSnapList),
  _zxSpectrumTapeList(zxSpectrumTapeList),
  _quickSave(quickSave)
{
  sort_keys();
}

void ZxSpectrumHidKeyboard::processHidReport(hid_keyboard_report_t const *report) {
  static hid_keyboard_report_t prev = { 0, 0, {0} };
  reset();
  if (report->keycode[0] == 1) return;
  const unsigned char m = report->modifier;
  // printf("hid:%02X %02X%02X%02X%02X%02X%02X\n", m, report->keycode[0], report->keycode[1], report->keycode[2], report->keycode[3], report->keycode[4], report->keycode[5]);

  if (m & 0x22) press(0, 0); // Shift
  if (m & 0x40) press(7, 1); // AtlGr -> Symbol

  unsigned int fkd = 0;
  unsigned int fkp = 0;
  
  for(unsigned int i = 0; i < 6; ++i) {
    const unsigned char hidKeyCode = report->keycode[i];
    
    for (int fk = 0; fk < 12; ++fk) {
      const unsigned int fkb = 1 << fk;
      const unsigned char fkc = HID_KEY_F1 + fk;
      if (hidKeyCode == fkc) {
        fkd |= fkb;
        if (!isInReport(&prev, fkc)) fkp |= fkb;
      }
    }

    const ZxSpectrumHidKey *k = findKey(hidKeyCode);
    if (k) {
      press(k->line, k->key);
    }
  }

  if (m & KEYBOARD_MODIFIER_LEFTCTRL) {
    for (int i = 0; i < 12; ++i) {
      if (fkp & (1 << i)) {
        printf("left ctrl F%d pressed\n", i);
        _quickSave->save(_ZxSpectrum, i);
      }
    }
  }
  else if (m & KEYBOARD_MODIFIER_LEFTALT) {
    for (int i = 0; i < 12; ++i) {
      if (fkp & (1 << i)) {
        printf("left alt F%d pressed\n", i);
        _quickSave->load(_ZxSpectrum, i);
      }
    }
  }
  else {
    // F11 & F12 both down together for reset
    if (((fkd & (3 << 10)) == (3 << 10)) && ((fkp & (3 << 10)) != 0)) _ZxSpectrum->reset();
    // F4 toggle moderate
    if (fkp & (1 << 3)) _ZxSpectrum->toggleModerate();
    // F8 curr snap
    if (fkp & (1 << 7)) _zxSpectrumSnapList->curr(_ZxSpectrum);
    // F9 previous snap
    if (fkp & (1 << 8)) _zxSpectrumSnapList->prev(_ZxSpectrum);
    // F10 next snap
    if (fkp & (1 << 9)) _zxSpectrumSnapList->next(_ZxSpectrum);

    // F5 curr tape
    if (fkp & (1 << 4)) _zxSpectrumTapeList->curr(_ZxSpectrum);
    // F6 previous tape
    if (fkp & (1 << 5)) _zxSpectrumTapeList->prev(_ZxSpectrum);
    // F7 next tape
    if (fkp & (1 << 6)) _zxSpectrumTapeList->next(_ZxSpectrum);

  }
  
  prev = *report;
}
