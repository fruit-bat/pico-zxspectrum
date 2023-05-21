// You may use, distribute and modify this code under the
// terms of the GPLv2 license, which unfortunately won't be
// written for another century.
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// See:
// http://www.vetra.com/scancodes.html
// https://wiki.osdev.org/PS/2_Keyboard
//
#include "ps2kbd_mrmltr.h"
#include "ps2kbd_mrmltr.pio.h"
#include "hardware/clocks.h"

#ifdef DEBUG_PS2
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

#define HID_KEYBOARD_REPORT_MAX_KEYS 6

// PS/2 set 2 to HID key conversion
static uint8_t ps2kbd_page_0[] {
  /* 00 (  0) */ HID_KEY_NONE,
  /* 01 (  1) */ HID_KEY_F9,
  /* 02 (  2) */ 0x00,
  /* 03 (  3) */ HID_KEY_F5,
  /* 04 (  4) */ HID_KEY_F3,
  /* 05 (  5) */ HID_KEY_F1,
  /* 06 (  6) */ HID_KEY_F2,
  /* 07 (  7) */ HID_KEY_F12,
  /* 08 (  8) */ HID_KEY_F13,
  /* 09 (  9) */ HID_KEY_F10,
  /* 0A ( 10) */ HID_KEY_F8,
  /* 0B ( 11) */ HID_KEY_F6,
  /* 0C ( 12) */ HID_KEY_F4,
  /* 0D ( 13) */ HID_KEY_TAB,
  /* 0E ( 14) */ HID_KEY_GRAVE,
  /* 0F ( 15) */ HID_KEY_KEYPAD_EQUAL,
  /* 10 ( 16) */ HID_KEY_F14,
  /* 11 ( 17) */ HID_KEY_ALT_LEFT,
  /* 12 ( 18) */ HID_KEY_SHIFT_LEFT,
  /* 13 ( 19) */ 0x00,
  /* 14 ( 20) */ HID_KEY_CONTROL_LEFT,
  /* 15 ( 21) */ HID_KEY_Q,
  /* 16 ( 22) */ HID_KEY_1,
  /* 17 ( 23) */ 0x00,
  /* 18 ( 24) */ HID_KEY_F15,
  /* 19 ( 25) */ 0x00,
  /* 1A ( 26) */ HID_KEY_Z,
  /* 1B ( 27) */ HID_KEY_S,
  /* 1C ( 28) */ HID_KEY_A,
  /* 1D ( 29) */ HID_KEY_W,
  /* 1E ( 30) */ HID_KEY_2,
  /* 1F ( 31) */ 0x00,
  /* 20 ( 32) */ HID_KEY_F16,
  /* 21 ( 33) */ HID_KEY_C,
  /* 22 ( 34) */ HID_KEY_X,
  /* 23 ( 35) */ HID_KEY_D,
  /* 24 ( 36) */ HID_KEY_E,
  /* 25 ( 37) */ HID_KEY_4,
  /* 26 ( 38) */ HID_KEY_3,
  /* 27 ( 39) */ 0x00,
  /* 28 ( 40) */ HID_KEY_F17,
  /* 29 ( 41) */ HID_KEY_SPACE,
  /* 2A ( 42) */ HID_KEY_V,
  /* 2B ( 43) */ HID_KEY_F,
  /* 2C ( 44) */ HID_KEY_T,
  /* 2D ( 45) */ HID_KEY_R,
  /* 2E ( 46) */ HID_KEY_5,
  /* 2F ( 47) */ 0x00,
  /* 30 ( 48) */ HID_KEY_F18,
  /* 31 ( 49) */ HID_KEY_N,
  /* 32 ( 50) */ HID_KEY_B,
  /* 33 ( 51) */ HID_KEY_H,
  /* 34 ( 52) */ HID_KEY_G,
  /* 35 ( 53) */ HID_KEY_Y,
  /* 36 ( 54) */ HID_KEY_6,
  /* 37 ( 55) */ 0x00,
  /* 38 ( 56) */ HID_KEY_F19,
  /* 39 ( 57) */ 0x00,
  /* 3A ( 58) */ HID_KEY_M,
  /* 3B ( 59) */ HID_KEY_J,
  /* 3C ( 60) */ HID_KEY_U,
  /* 3D ( 61) */ HID_KEY_7,
  /* 3E ( 62) */ HID_KEY_8,
  /* 3F ( 63) */ 0x00,
  /* 40 ( 64) */ HID_KEY_F20,
  /* 41 ( 65) */ HID_KEY_COMMA,
  /* 42 ( 66) */ HID_KEY_K,
  /* 43 ( 67) */ HID_KEY_I,
  /* 44 ( 68) */ HID_KEY_O,
  /* 45 ( 69) */ HID_KEY_0,
  /* 46 ( 70) */ HID_KEY_9,
  /* 47 ( 71) */ 0x00,
  /* 48 ( 72) */ HID_KEY_F21,
  /* 49 ( 73) */ HID_KEY_PERIOD,
  /* 4A ( 74) */ HID_KEY_SLASH,
  /* 4B ( 75) */ HID_KEY_L,
  /* 4C ( 76) */ HID_KEY_SEMICOLON,
  /* 4D ( 77) */ HID_KEY_P,
  /* 4E ( 78) */ HID_KEY_MINUS,
  /* 4F ( 79) */ 0x00,
  /* 50 ( 80) */ HID_KEY_F22,
  /* 51 ( 81) */ 0x00,
  /* 52 ( 82) */ HID_KEY_APOSTROPHE,
  /* 53 ( 83) */ 0x00,
  /* 54 ( 84) */ HID_KEY_BRACKET_LEFT,
  /* 55 ( 85) */ HID_KEY_EQUAL,
  /* 56 ( 86) */ 0x00,
  /* 57 ( 87) */ HID_KEY_F23,
  /* 58 ( 88) */ HID_KEY_CAPS_LOCK,
  /* 59 ( 89) */ HID_KEY_SHIFT_RIGHT,
  /* 5A ( 90) */ HID_KEY_ENTER, // RETURN ??
  /* 5B ( 91) */ HID_KEY_BRACKET_RIGHT,
  /* 5C ( 92) */ 0x00,
  /* 5D ( 93) */ HID_KEY_EUROPE_1,
  /* 5E ( 94) */ 0x00,
  /* 5F ( 95) */ HID_KEY_F24,
  /* 60 ( 96) */ 0x00,
  /* 61 ( 97) */ HID_KEY_EUROPE_2,
  /* 62 ( 98) */ 0x00,
  /* 63 ( 99) */ 0x00,
  /* 64 (100) */ 0x00,
  /* 65 (101) */ 0x00,
  /* 66 (102) */ HID_KEY_BACKSPACE,
  /* 67 (103) */ 0x00,
  /* 68 (104) */ 0x00,
  /* 69 (105) */ HID_KEY_KEYPAD_1,
  /* 6A (106) */ 0x00,
  /* 6B (107) */ HID_KEY_KEYPAD_4,
  /* 6C (108) */ HID_KEY_KEYPAD_7,
  /* 6D (109) */ 0x00,
  /* 6E (110) */ 0x00,
  /* 6F (111) */ 0x00,
  /* 70 (112) */ HID_KEY_KEYPAD_0,
  /* 71 (113) */ HID_KEY_KEYPAD_DECIMAL,
  /* 72 (114) */ HID_KEY_KEYPAD_2,
  /* 73 (115) */ HID_KEY_KEYPAD_5,
  /* 74 (116) */ HID_KEY_KEYPAD_6,
  /* 75 (117) */ HID_KEY_KEYPAD_8,
  /* 76 (118) */ HID_KEY_ESCAPE,
  /* 77 (119) */ HID_KEY_NUM_LOCK,
  /* 78 (120) */ HID_KEY_F11,
  /* 79 (121) */ HID_KEY_KEYPAD_ADD,
  /* 7A (122) */ HID_KEY_KEYPAD_3,
  /* 7B (123) */ HID_KEY_KEYPAD_SUBTRACT,
  /* 7C (124) */ HID_KEY_KEYPAD_MULTIPLY,
  /* 7D (125) */ HID_KEY_KEYPAD_9,
  /* 7E (126) */ HID_KEY_SCROLL_LOCK,
  /* 7F (127) */ 0x00,
  /* 80 (128) */ 0x00,
  /* 81 (129) */ 0x00,
  /* 82 (130) */ 0x00,
  /* 83 (131) */ HID_KEY_F7
};

Ps2Kbd_Mrmltr::Ps2Kbd_Mrmltr(PIO pio, uint base_gpio, std::function<void(hid_keyboard_report_t *curr, hid_keyboard_report_t *prev)> keyHandler) :
  _pio(pio),
  _base_gpio(base_gpio),
  _double(false),
  _overflow(false),
  _keyHandler(keyHandler)
{
  clearHidKeys();
  clearActions();
}

void Ps2Kbd_Mrmltr::clearHidKeys() {
  _report.modifier = 0;
  for (int i = 0; i < HID_KEYBOARD_REPORT_MAX_KEYS; ++i) _report.keycode[i] = HID_KEY_NONE;
}

inline static uint8_t hidKeyToMod(uint8_t hidKeyCode) {
  uint8_t m = 0;
  switch(hidKeyCode) {
  case HID_KEY_CONTROL_LEFT: m = KEYBOARD_MODIFIER_LEFTCTRL; break;
  case HID_KEY_SHIFT_LEFT: m = KEYBOARD_MODIFIER_LEFTSHIFT; break;
  case HID_KEY_ALT_LEFT: m = KEYBOARD_MODIFIER_LEFTALT; break;
  case HID_KEY_GUI_LEFT: m = KEYBOARD_MODIFIER_LEFTGUI; break;
  case HID_KEY_CONTROL_RIGHT: m = KEYBOARD_MODIFIER_RIGHTCTRL; break;
  case HID_KEY_SHIFT_RIGHT: m = KEYBOARD_MODIFIER_RIGHTSHIFT; break;
  case HID_KEY_ALT_RIGHT: m = KEYBOARD_MODIFIER_RIGHTALT; break;
  case HID_KEY_GUI_RIGHT: m = KEYBOARD_MODIFIER_RIGHTGUI; break;
  default: break;
  } 
  return m;
}

void Ps2Kbd_Mrmltr::handleHidKeyPress(uint8_t hidKeyCode) {
  hid_keyboard_report_t prev = _report;
  
  // Check the key is not alreay pressed
  for (int i = 0; i < HID_KEYBOARD_REPORT_MAX_KEYS; ++i) {
    if (_report.keycode[i] == hidKeyCode) {
      return;
    }
  }
  
  _report.modifier |= hidKeyToMod(hidKeyCode);
  
  for (int i = 0; i < HID_KEYBOARD_REPORT_MAX_KEYS; ++i) {
    if (_report.keycode[i] == HID_KEY_NONE) {
      _report.keycode[i] = hidKeyCode;      
      _keyHandler(&_report, &prev);
      return;
    }
  }
  
  // TODO Overflow
  DBG_PRINTF("PS/2 keyboard HID overflow\n");
}

void Ps2Kbd_Mrmltr::handleHidKeyRelease(uint8_t hidKeyCode) {
  hid_keyboard_report_t prev = _report;
  
  _report.modifier &= ~hidKeyToMod(hidKeyCode);
  
  for (int i = 0; i < HID_KEYBOARD_REPORT_MAX_KEYS; ++i) {
    if (_report.keycode[i] == hidKeyCode) {
      _report.keycode[i] = HID_KEY_NONE;
      _keyHandler(&_report, &prev);
      return;
    }
  }
}

uint8_t Ps2Kbd_Mrmltr::hidCodePage0(uint8_t ps2code) {
  return ps2code < sizeof(ps2kbd_page_0) ? ps2kbd_page_0[ps2code] : HID_KEY_NONE;
}

// PS/2 set 2 after 0xe0 to HID key conversion
uint8_t Ps2Kbd_Mrmltr::hidCodePage1(uint8_t ps2code) {
  switch(ps2code) {
// TODO these belong to a different HID usage page
//  case 0x37: return HID_KEY_POWER;
//  case 0x3f: return HID_KEY_SLEEP;
//  case 0x5e: return HID_KEY_WAKE;
  case 0x11: return HID_KEY_ALT_RIGHT;
  case 0x1f: return HID_KEY_GUI_LEFT;
  case 0x14: return HID_KEY_CONTROL_RIGHT;
  case 0x27: return HID_KEY_GUI_RIGHT;
  case 0x4a: return HID_KEY_KEYPAD_DIVIDE;
  case 0x5a: return HID_KEY_KEYPAD_ENTER;
  case 0x69: return HID_KEY_END;
  case 0x6b: return HID_KEY_ARROW_LEFT;
  case 0x6c: return HID_KEY_HOME;
  case 0x7c: return HID_KEY_PRINT_SCREEN;
  case 0x70: return HID_KEY_INSERT;
  case 0x71: return HID_KEY_DELETE;
  case 0x72: return HID_KEY_ARROW_DOWN;
  case 0x74: return HID_KEY_ARROW_RIGHT;
  case 0x75: return HID_KEY_ARROW_UP;
  case 0x7a: return HID_KEY_PAGE_DOWN;
  case 0x7d: return HID_KEY_PAGE_UP;

  default: 
    return HID_KEY_NONE;
  }
}

void Ps2Kbd_Mrmltr::handleActions() {
  #ifdef DEBUG_PS2
  for (uint i = 0; i <= _action; ++i) {
    DBG_PRINTF("PS/2 key %s page %2.2X (%3.3d) code %2.2X (%3.3d)\n",
      _actions[i].release ? "release" : "press",
      _actions[i].page,
      _actions[i].page,
      _actions[i].code,
      _actions[i].code);
  }
  #endif
  
  uint8_t hidCode;
  bool release;
  if (_action == 0) {
    switch (_actions[0].page) {
      case 1: {
        hidCode = hidCodePage1(_actions[0].code);
        break; 
      }
      default: {
        hidCode = hidCodePage0(_actions[0].code);
        break;
      }
    }
    release = _actions[0].release;
  }
  else {
    // TODO get the HID code for extended PS/2 codes
    hidCode = HID_KEY_NONE;
    release = false;
  }
  
  if (hidCode != HID_KEY_NONE) {
    
    DBG_PRINTF("HID key %s code %2.2X (%3.3d)\n",
      release ? "release" : "press",
      hidCode,
      hidCode);
      
    if (release) {
      handleHidKeyRelease(hidCode);
    }
    else {
      handleHidKeyPress(hidCode);
    }
  }
  
  DBG_PRINTF("PS/2 HID m=%2X ", _report.modifier);
  #ifdef DEBUG_PS2
  for (int i = 0; i < HID_KEYBOARD_REPORT_MAX_KEYS; ++i) printf("%2X ", _report.keycode[i]);
  printf("\n");
  #endif
}

void Ps2Kbd_Mrmltr::tick() {
  if (pio_sm_is_rx_fifo_full(_pio, _sm)) {
    DBG_PRINTF("PS/2 keyboard PIO overflow\n");
    _overflow = true;
    while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
      // pull a scan code from the PIO SM fifo
      uint32_t rc = _pio->rxf[_sm];    
      printf("PS/2 drain rc %4.4lX (%ld)\n", rc, rc);
    }
    clearHidKeys();
    clearActions();
  }
  
  while (!pio_sm_is_rx_fifo_empty(_pio, _sm)) {
    // pull a scan code from the PIO SM fifo
    uint32_t rc = _pio->rxf[_sm];    
    DBG_PRINTF("PS/2 rc %4.4lX (%ld)\n", rc, rc);
    
    uint32_t code = (rc << 2) >> 24;
    DBG_PRINTF("PS/2 keycode %2.2lX (%ld)\n", code, code);

    // TODO Handle PS/2 overflow/error messages
    switch (code) {
      case 0xaa: {
         DBG_PRINTF("PS/2 keyboard Self test passed\n");
         break;       
      }
      case 0xe1: {
        _double = true;
        break;
      }
      case 0xe0: {
        _actions[_action].page = 1;
        break;
      }
      case 0xf0: {
        _actions[_action].release = true;
        break;
      }
      default: {
        _actions[_action].code = code;
        if (_double) {
          _action = 1;
          _double = false;
        }
        else {
          handleActions();
          clearActions();
        }
        break;
      }
    }
  }
}

// TODO Error checking and reporting
void Ps2Kbd_Mrmltr::init_gpio() {
    // init KBD pins to input
    gpio_init(_base_gpio);     // Data
    gpio_init(_base_gpio + 1); // Clock
    // with pull up
    gpio_pull_up(_base_gpio);
    gpio_pull_up(_base_gpio + 1);
    // get a state machine
    _sm = pio_claim_unused_sm(_pio, true);
    // reserve program space in SM memory
    uint offset = pio_add_program(_pio, &ps2kbd_program);
    // Set pin directions base
    pio_sm_set_consecutive_pindirs(_pio, _sm, _base_gpio, 2, false);
    // program the start and wrap SM registers
    pio_sm_config c = ps2kbd_program_get_default_config(offset);
    // Set the base input pin. pin index 0 is DAT, index 1 is CLK  // Murmulator: 0->CLK 1->DAT ( _base_gpio + 1)
    //  sm_config_set_in_pins(&c, _base_gpio);
    sm_config_set_in_pins(&c, _base_gpio + 1);
    // Shift 8 bits to the right, autopush enabled
    sm_config_set_in_shift(&c, true, true, 10);
    // Deeper FIFO as we're not doing any TX
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    // We don't expect clock faster than 16.7KHz and want no less
    // than 8 SM cycles per keyboard clock.
    float div = (float)clock_get_hz(clk_sys) / (8 * 16700);
    sm_config_set_clkdiv(&c, div);
    // Ready to go
    pio_sm_init(_pio, _sm, offset, &c);
    pio_sm_set_enabled(_pio, _sm, true);
}
