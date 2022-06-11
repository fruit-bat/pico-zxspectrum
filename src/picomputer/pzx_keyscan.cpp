#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "class/hid/hid.h"
#include "pzx_keyscan.h"

#define SAMPLES 4 

#define CP_VGA 20, 21, 22, 26, 27, 28
#define CP_MAX 1, 2, 3, 4, 5, 14
#define RP_VGA 14, 15, 16, 17, 18, 19
#define RP_MAX 6, 9, 15, 8, 7, 22
#define CP_SHIFT_VGA 20
#define CP_SHIFT_MAX 1
#define CP_JOIN_VGA(a) ((a & 7) | ((a >> 3) & (7 << 3)))
#define CP_JOIN_MAX(a) ((a & 31) | ((a >> 8) & 32))


#ifdef PICOMPUTER_VGA
  #define CP CP_VGA
  #define RP RP_VGA
  #define CP_JOIN CP_JOIN_VGA
  #define CP_SHIFT CP_SHIFT_VGA
#endif
#if defined(PICOMPUTER_MAX) || defined(PICOMPUTER_ZX)
  #define CP CP_MAX
  #define RP RP_MAX
  #define CP_JOIN CP_JOIN_MAX
  #define CP_SHIFT CP_SHIFT_MAX
#endif

static uint8_t cp[] = {CP};                      // Column pins
static uint8_t rp[] = {RP};                      // Row pins
static uint8_t rs[6][SAMPLES];                   // Oversampled pins
static uint8_t rdb[6];                           // Debounced pins
static hid_keyboard_report_t hr[2];              // Current and previous hid report
static uint8_t hri = 0;                          // Currenct hid report index
static uint8_t kbi = 0;
static uint8_t kbits[3][6][6] = { 
  // Normal mappings
  {
    // Row 0
    { HID_KEY_SPACE, HID_KEY_ALT_RIGHT, HID_KEY_M, HID_KEY_N, HID_KEY_B, HID_KEY_ARROW_DOWN },
    // Row 1
    { HID_KEY_ENTER, HID_KEY_L, HID_KEY_K, HID_KEY_J, HID_KEY_H, HID_KEY_ARROW_LEFT },
    // Row 2
    { HID_KEY_P, HID_KEY_O, HID_KEY_I, HID_KEY_U, HID_KEY_Y, HID_KEY_ARROW_UP },
    // Row 3
    { HID_KEY_BACKSPACE, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_ARROW_RIGHT },
    // Row 4
    { HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_ESCAPE },
    // Row 5
    { HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, /* HID_KEY_ALT_LEFT */ 0}
  },
  // Numeric mappings
  {
    // Row 0
    { HID_KEY_SPACE, HID_KEY_ALT_RIGHT, HID_KEY_SEMICOLON, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_ARROW_DOWN },
    // Row 1
    { HID_KEY_ENTER, HID_KEY_BRACKET_RIGHT, HID_KEY_BRACKET_LEFT, HID_KEY_GRAVE, HID_KEY_BACKSLASH, HID_KEY_ARROW_LEFT },
    // Row 2
    { HID_KEY_0, HID_KEY_9, HID_KEY_8, HID_KEY_7, HID_KEY_6, HID_KEY_ARROW_UP },
    // Row 3
    { HID_KEY_BACKSPACE, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_APOSTROPHE, HID_KEY_ARROW_RIGHT },
    // Row 4
    { HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_ESCAPE },
    // Row 5
    { HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, /* HID_KEY_ALT_LEFT */ 0 }
  },
  // Alt down mappings
  {
    // Row 0 (Quick save 11-12)
    { 0, 0, 0, HID_KEY_F12, HID_KEY_F11, 0 },
    // Row 1 (Quick save 6-10)
    { HID_KEY_F10, HID_KEY_F9, HID_KEY_F8, HID_KEY_F7, HID_KEY_F6, 0 },
    // Row 2 (Quick save 1-5)
    { HID_KEY_F5, HID_KEY_F4, HID_KEY_F3, HID_KEY_F2, HID_KEY_F1, 0 },
    // Row 3
    { 0, 0, 0, 0, 0, 0 },
    // Row 4
    { HID_KEY_F8, HID_KEY_F9, HID_KEY_F10, 0, 0, 0 },
    // Row 5 (Menu, 0, ZX Symbol)
    { HID_KEY_F1, 0, 0, 0, 0, /* HID_KEY_ALT_LEFT */ 0 }
  }
};

#define KEY_ALT_ROW 5
#define KEY_ALT_BIT 0x20
#define KEY_ESC_ROW 4
#define KEY_ESC_BIT 0x20
#define KEY_UP_ROW 2
#define KEY_UP_BIT 0x20
#define KEY_DOWN_ROW 0
#define KEY_DOWN_BIT 0x20
#define KEY_LEFT_ROW 1
#define KEY_LEFT_BIT 0x20
#define KEY_RIGHT_ROW 3
#define KEY_RIGHT_BIT 0x20
#define KEY_ENTER_ROW 1
#define KEY_ENTER_BIT 0x01

void pzx_keyscan_init() {
  
    for(int i = 0; i < 6; ++i) {
      gpio_init(rp[i]);
      gpio_set_dir(rp[i], GPIO_IN);    
      gpio_disable_pulls(rp[i]);
   }  
    
   for(int i = 0; i < 6; ++i) {
      gpio_init(cp[i]);
      gpio_set_dir(cp[i], GPIO_IN);    
      gpio_pull_up(cp[i]);
   }
   
   uint32_t row = rp[0];
   gpio_set_dir(row, GPIO_OUT);
   gpio_put(row, 0);
}

void __not_in_flash_func(pzx_keyscan_row)() {
  static uint32_t ri = 0;
  static uint32_t si = 0;
  uint32_t a = ~(gpio_get_all() >> CP_SHIFT);

  uint32_t r = CP_JOIN(a);
  rs[ri][si] = (uint8_t)r;
  uint32_t row;
  row = rp[ri];
  gpio_set_dir(row, GPIO_IN);
  gpio_disable_pulls(row);
  if (++ri >= 6) {
    ri = 0;
    if (++si >= SAMPLES) si = 0;
  }
  row = rp[ri];
  gpio_set_dir(row, GPIO_OUT);
  gpio_put(row, 0);
  
  uint32_t om = 0;
  uint32_t am = 0x3f;
  for(int si = 0; si < SAMPLES; ++si) {
    uint8_t s = rs[ri][si];
    om |= s; // bits 0 if no samples have the button pressed
    am &= s; // bits 1 if all samples have the button pressed
  }
  // only change key state if all samples on or off
  rdb[ri] = (am | rdb[ri]) & om; 
}

uint8_t __not_in_flash_func(pzx_kempston)() {
  return 
    ((rdb[KEY_ESC_ROW] & KEY_ESC_BIT) >> 1)      // Kempston fire
  | ((rdb[KEY_UP_ROW] & KEY_UP_BIT) >> 2)        // Kempston up
  | ((rdb[KEY_DOWN_ROW] & KEY_DOWN_BIT) >> 3)    // Kempston down
  | ((rdb[KEY_LEFT_ROW] & KEY_LEFT_BIT) >> 4)    // Kempston left
  | ((rdb[KEY_RIGHT_ROW] & KEY_RIGHT_BIT) >> 5); // Kempston right
}

void __not_in_flash_func(pzx_keyscan_get_hid_reports)(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev) {

  // Key modifier (shift, alt, ctrl, etc.)
  static uint8_t modifier = 0;
  
  bool alt_down = rdb[KEY_ALT_ROW] & KEY_ALT_BIT;
  
  if (alt_down) {
    if (rdb[KEY_UP_ROW] & KEY_UP_BIT) {
      // Shift on
      modifier |= 2;
    }
    else if (rdb[KEY_DOWN_ROW] & KEY_DOWN_BIT) {
      // Shift off
      modifier &= ~2;
    }
    if (rdb[KEY_RIGHT_ROW] & KEY_RIGHT_BIT) {
      // Numeric keyboard on
      kbi = 1;
    }
    else if (rdb[KEY_LEFT_ROW] & KEY_LEFT_BIT) {
      // Numeric keyboard off
      kbi = 0;
    }
    rdb[KEY_UP_ROW] &= ~KEY_UP_BIT;
    rdb[KEY_DOWN_ROW] &= ~KEY_DOWN_BIT;
    rdb[KEY_RIGHT_ROW] &= ~KEY_RIGHT_BIT;
    rdb[KEY_LEFT_ROW] &= ~KEY_LEFT_BIT;
  }
  
  // Build the current hid report
  uint32_t ki = 0;
  hid_keyboard_report_t *chr = &hr[hri & 1];
  chr->modifier = modifier;
  // Press ctrl for quick save
  if (alt_down && (((rdb[0] | rdb[1] | rdb[2]) & 31) != 0)) chr->modifier |= 1;
  for(int ri = 0; ri < 6; ++ri) {
    uint8_t r = rdb[ri];
    uint32_t ci = 0;
    while(r) {
      if (r & 1) {
        if (ki >= sizeof(chr->keycode)) {
          // overflow
          ki = 0;
          while(ki < sizeof(chr->keycode)) chr->keycode[ki++] = 1;
          break;  
        }
        uint8_t kc = kbits[alt_down ? 2 : kbi][ri][ci];
        if (kc) chr->keycode[ki++] = kc;
      }
      ++ci;
      r >>= 1;
    }
  }
  while(ki < sizeof(chr->keycode)) chr->keycode[ki++] = 0;
  *curr = chr;
  hri++;
  *prev = &hr[hri & 1];
}

