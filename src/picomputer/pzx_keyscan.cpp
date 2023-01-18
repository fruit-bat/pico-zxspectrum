#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "class/hid/hid.h"
#include "pzx_keyscan.h"

// define REAL_ZXKEYBOARD to use ZX Spectum 5x8 real keyboard matrix

#define SAMPLES 4 

#define CN_VGA 6
#define CN_MAX 6
#define CN_PZX 7
#define CN_PZX_REAL 8
#define RN_VGA 6
#define RN_MAX 6
#define RN_PZX 7
#define RN_PZX_REAL 6
#define CP_VGA 20, 21, 22, 26, 27, 28
#define CP_MAX 1, 2, 3, 4, 5, 14
#define CP_PZX 19, 20, 21, 22, 26, 27, 28
#define CP_PZX_REAL 18, 19, 20, 21, 22, 26, 27, 28
#define RP_VGA 14, 15, 16, 17, 18, 19
#define RP_MAX 6, 9, 15, 8, 7, 22
#define RP_PZX 8, 9, 14, 15, 16, 17, 18
#define RP_PZX_REAL 8, 9, 14, 15, 16, 17
#define CP_SHIFT_VGA 20
#define CP_SHIFT_MAX 1
#define CP_SHIFT_PZX 19
#define CP_SHIFT_PZX_REAL 18
#define CP_JOIN_VGA(a) ((a & 7) | ((a >> 3) & (7 << 3)))
#define CP_JOIN_MAX(a) ((a & 31) | ((a >> 8) & 32))
#define CP_JOIN_PZX(a) ((a & 15) | ((a >> 3) & (7 << 4)))
#define CP_JOIN_PZX_REAL(a) ((a & 31) | ((a >> 3) & (7 << 5)))

#ifdef PICOMPUTER_VGA
  #define CP CP_VGA
  #define RP RP_VGA
  #define CP_JOIN CP_JOIN_VGA
  #define CP_SHIFT CP_SHIFT_VGA
  #define RN RN_VGA
  #define CN CN_VGA
#endif
#if defined(PICOMPUTER_MAX) || defined(PICOMPUTER_ZX)
  #define CP CP_MAX
  #define RP RP_MAX
  #define CP_JOIN CP_JOIN_MAX
  #define CP_SHIFT CP_SHIFT_MAX
  #define RN RN_MAX
  #define CN CN_MAX
#endif
#ifdef PICOMPUTER_PICOZX
  #ifdef REAL_ZXKEYBOARD
    #define CP CP_PZX_REAL
    #define RP RP_PZX_REAL
    #define CP_JOIN CP_JOIN_PZX_REAL
    #define CP_SHIFT CP_SHIFT_PZX_REAL
    #define RN RN_PZX_REAL
    #define CN CN_PZX_REAL
  #else
    #define CP CP_PZX
    #define RP RP_PZX
    #define CP_JOIN CP_JOIN_PZX
    #define CP_SHIFT CP_SHIFT_PZX
    #define RN RN_PZX
    #define CN CN_PZX
  #endif
#endif

static uint8_t cp[] = {CP};                      // Column pins
static uint8_t rp[] = {RP};                      // Row pins
static uint8_t rs[RN][SAMPLES];                  // Oversampled pins
static uint8_t rdb[RN];                          // Debounced pins
static hid_keyboard_report_t hr[2];              // Current and previous hid report
static uint8_t hri = 0;                          // Currenct hid report index
static uint8_t kbi = 0;

//Keyboard Matrix Arrays [INDEX][ROWS][COLS]
#ifdef PICOMPUTER_PICOZX
#ifdef REAL_ZXKEYBOARD
static uint8_t kbits[5][6][8] = { 
  // Normal mappings + cursor SymShift = HID_KEY_ALT_RIGHT
  {
    { HID_KEY_ENTER, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_ARROW_RIGHT, HID_KEY_ARROW_DOWN, HID_KEY_F1, HID_KEY_F13, HID_KEY_F14 },
    { HID_KEY_B, HID_KEY_H, HID_KEY_V, HID_KEY_Y, HID_KEY_6, HID_KEY_G, HID_KEY_T, HID_KEY_5 },
    { HID_KEY_N, HID_KEY_J, HID_KEY_C, HID_KEY_U, HID_KEY_7, HID_KEY_F, HID_KEY_R, HID_KEY_4 },
    { HID_KEY_M, HID_KEY_K, HID_KEY_X, HID_KEY_I, HID_KEY_8, HID_KEY_D, HID_KEY_E, HID_KEY_3 },
    { HID_KEY_ALT_RIGHT, HID_KEY_L, HID_KEY_Z, HID_KEY_O, HID_KEY_9, HID_KEY_S, HID_KEY_W, HID_KEY_2 },
    { HID_KEY_SPACE, HID_KEY_ENTER, HID_KEY_SHIFT_LEFT, HID_KEY_P, HID_KEY_0, HID_KEY_A, HID_KEY_Q, HID_KEY_1 }
  },
  // Shifted normal mappings + cursor
  {
    { HID_KEY_ESCAPE, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_ARROW_RIGHT, HID_KEY_ARROW_DOWN, HID_KEY_F1, HID_KEY_F3, HID_KEY_F4 },
    { HID_KEY_B, HID_KEY_H, HID_KEY_V, HID_KEY_Y, HID_KEY_6, HID_KEY_G, HID_KEY_T, HID_KEY_5 },
    { HID_KEY_N, HID_KEY_J, HID_KEY_C, HID_KEY_U, HID_KEY_7, HID_KEY_F, HID_KEY_R, HID_KEY_4 },
    { HID_KEY_M, HID_KEY_K, HID_KEY_X, HID_KEY_I, HID_KEY_8, HID_KEY_D, HID_KEY_E, HID_KEY_3 },
    { HID_KEY_ALT_RIGHT, HID_KEY_L, HID_KEY_Z, HID_KEY_O, HID_KEY_9, HID_KEY_S, HID_KEY_W, HID_KEY_2 },
    { HID_KEY_SPACE, HID_KEY_ENTER, HID_KEY_SHIFT_LEFT, HID_KEY_P, HID_KEY_0, HID_KEY_A, HID_KEY_Q, HID_KEY_1 }
  },
  // Normal mappings + joystick
  {
    { 0, 0, 0, 0, 0, HID_KEY_F1, HID_KEY_F3, HID_KEY_F4 },
    { HID_KEY_B, HID_KEY_H, HID_KEY_V, HID_KEY_Y, HID_KEY_6, HID_KEY_G, HID_KEY_T, HID_KEY_5 },
    { HID_KEY_N, HID_KEY_J, HID_KEY_C, HID_KEY_U, HID_KEY_7, HID_KEY_F, HID_KEY_R, HID_KEY_4 },
    { HID_KEY_M, HID_KEY_K, HID_KEY_X, HID_KEY_I, HID_KEY_8, HID_KEY_D, HID_KEY_E, HID_KEY_3 },
    { HID_KEY_ALT_RIGHT, HID_KEY_L, HID_KEY_Z, HID_KEY_O, HID_KEY_9, HID_KEY_S, HID_KEY_W, HID_KEY_2 },
    { HID_KEY_SPACE, HID_KEY_ENTER, HID_KEY_SHIFT_LEFT, HID_KEY_P, HID_KEY_0, HID_KEY_A, HID_KEY_Q, HID_KEY_1 }
  },
  // Shifted normal mappings + joystick
  {
    { 0, 0, 0, 0, 0, HID_KEY_F1, HID_KEY_F3, HID_KEY_F4 },
    { HID_KEY_B, HID_KEY_H, HID_KEY_V, HID_KEY_Y, HID_KEY_6, HID_KEY_G, HID_KEY_T, HID_KEY_5 },
    { HID_KEY_N, HID_KEY_J, HID_KEY_C, HID_KEY_U, HID_KEY_7, HID_KEY_F, HID_KEY_R, HID_KEY_4 },
    { HID_KEY_M, HID_KEY_K, HID_KEY_X, HID_KEY_I, HID_KEY_8, HID_KEY_D, HID_KEY_E, HID_KEY_3 },
    { HID_KEY_ALT_RIGHT, HID_KEY_L, HID_KEY_Z, HID_KEY_O, HID_KEY_9, HID_KEY_S, HID_KEY_W, HID_KEY_2 },
    { HID_KEY_SPACE, HID_KEY_ENTER, HID_KEY_SHIFT_LEFT, HID_KEY_P, HID_KEY_0, HID_KEY_A, HID_KEY_Q, HID_KEY_1 }
  }
};
#else
static uint8_t kbits[5][7][7] = { 
  // Normal mappings + cursor SymShift = HID_KEY_ALT_RIGHT
  {
    { HID_KEY_ENTER, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_ARROW_RIGHT, HID_KEY_ARROW_DOWN, 0, HID_KEY_ALT_RIGHT },
    { HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7 },
    { HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R },
    { HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_A },
    { HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K },
    { HID_KEY_L, HID_KEY_ENTER, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B },
    { HID_KEY_N, HID_KEY_M, HID_KEY_SPACE, HID_KEY_F1, HID_KEY_F8, HID_KEY_F9, HID_KEY_F10 },
  },
  // Shifted normal mappings + cursor
  {
    { HID_KEY_ESCAPE, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_ARROW_RIGHT, HID_KEY_ARROW_DOWN, 0, HID_KEY_ALT_RIGHT },
    { HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7 },
    { HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R },
    { HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_A },
    { HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K },
    { HID_KEY_L, HID_KEY_ENTER, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B },
    { HID_KEY_N, HID_KEY_M, HID_KEY_SPACE, HID_KEY_F13, HID_KEY_F14, 0, 0 },
  },
  // Normal mappings + joystick
  {
    { 0, 0, 0, 0, 0, 0, HID_KEY_ALT_RIGHT },
    { HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7 },
    { HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R },
    { HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_A },
    { HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K },
    { HID_KEY_L, HID_KEY_ENTER, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B },
    { HID_KEY_N, HID_KEY_M, HID_KEY_SPACE, HID_KEY_F1, HID_KEY_F8, HID_KEY_F9, HID_KEY_F10 },
  },
  // Shifted normal mappings + joystick
  {
    { 0, 0, 0, 0, 0, 0, HID_KEY_ALT_RIGHT },
    { HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_7 },
    { HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R },
    { HID_KEY_T, HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_A },
    { HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_J, HID_KEY_K },
    { HID_KEY_L, HID_KEY_ENTER, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B },
    { HID_KEY_N, HID_KEY_M, HID_KEY_SPACE, HID_KEY_F13, HID_KEY_F14, 0, 0 },
  }
};
#endif
#else
static uint8_t kbits[5][6][6] = { 
  // Normal mappings + cursor joystick
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
  // Normal mappings + kempston joystick
  {
    // Row 0
    { HID_KEY_SPACE, HID_KEY_ALT_RIGHT, HID_KEY_M, HID_KEY_N, HID_KEY_B, 0 },
    // Row 1
    { HID_KEY_ENTER, HID_KEY_L, HID_KEY_K, HID_KEY_J, HID_KEY_H, 0 },
    // Row 2
    { HID_KEY_P, HID_KEY_O, HID_KEY_I, HID_KEY_U, HID_KEY_Y, 0 },
    // Row 3
    { HID_KEY_BACKSPACE, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, 0 },
    // Row 4
    { HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_ESCAPE },
    // Row 5
    { HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, /* HID_KEY_ALT_LEFT */ 0}
  },
  // Numeric mappings + cursor joystick
  {
    // Row 0
    { HID_KEY_SPACE, HID_KEY_ALT_RIGHT, HID_KEY_SEMICOLON, HID_KEY_MINUS, HID_KEY_EQUAL, 0 },
    // Row 1
    { HID_KEY_ENTER, HID_KEY_BRACKET_RIGHT, HID_KEY_BRACKET_LEFT, HID_KEY_GRAVE, HID_KEY_BACKSLASH, 0 },
    // Row 2
    { HID_KEY_0, HID_KEY_9, HID_KEY_8, HID_KEY_7, HID_KEY_6, 0 },
    // Row 3
    { HID_KEY_BACKSPACE, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_APOSTROPHE, 0 },
    // Row 4
    { HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, HID_KEY_ESCAPE },
    // Row 5
    { HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, /* HID_KEY_ALT_LEFT */ 0 }
  },
  // Numeric mappings + kempston joystick
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
    { 0, 0, 0, /* cursor mode (C) */ 0, /* joystick mode (V) */ 0, 0 },
    // Row 4
    { HID_KEY_F8, HID_KEY_F9, HID_KEY_F10, 0, 0, 0 },
    // Row 5 (Menu, 0, ZX Symbol)
    { HID_KEY_F1, HID_KEY_F2, HID_KEY_F3, HID_KEY_F4, 0, /* HID_KEY_ALT_LEFT */ 0 }
  }
};
#endif

#if defined(PICOMPUTER_MAX) || defined(PICOMPUTER_ZX) || defined(PICOMPUTER_VGA)
  #define KEY_ALT_ROW 5
  #define KEY_ALT_BIT 0x20
  #define KEY_FIRE_ROW 4
  #define KEY_FIRE_BIT 0x20
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
#else
  #ifdef REAL_ZXKEYBOARD
    #define KEY_UP_ROW 0
    #define KEY_UP_BIT 0x04
    #define KEY_DOWN_ROW 0
    #define KEY_DOWN_BIT 0x10
    #define KEY_LEFT_ROW 0
    #define KEY_LEFT_BIT 0x02
    #define KEY_RIGHT_ROW 0
    #define KEY_RIGHT_BIT 0x08
    #define KEY_FIRE_ROW 0
    #define KEY_FIRE_BIT 0x01
    #define KEY_SHIFT_ROW 5
    #define KEY_SHIFT_BIT 0x04
    #define KEY_CURSOR_ROW 6
    #define KEY_CURSOR_BIT 0x20
    #define KEY_KEMPSTON_ROW 6
    #define KEY_KEMPSTON_BIT 0x40 
    #define LED_PIN 25
  #else
    #define KEY_UP_ROW 0
    #define KEY_UP_BIT 0x04
    #define KEY_DOWN_ROW 0
    #define KEY_DOWN_BIT 0x10
    #define KEY_LEFT_ROW 0
    #define KEY_LEFT_BIT 0x02
    #define KEY_RIGHT_ROW 0
    #define KEY_RIGHT_BIT 0x08
    #define KEY_FIRE_ROW 0
    #define KEY_FIRE_BIT 0x01
    #define KEY_SHIFT_ROW 0
    #define KEY_SHIFT_BIT 0x20
  #endif  
#endif


void pzx_keyscan_init() {

  for(int i = 0; i < RN; ++i) {
      gpio_init(rp[i]);
      gpio_set_dir(rp[i], GPIO_IN);    
      gpio_disable_pulls(rp[i]);
   }  
    
   for(int i = 0; i < CN; ++i) {
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
  if (++ri >= RN) {
    ri = 0;
    if (++si >= SAMPLES) si = 0;
  }
  row = rp[ri];
  gpio_set_dir(row, GPIO_OUT);
  gpio_put(row, 0);
  
  uint32_t om = 0;
  uint32_t am = (1 << CN) - 1;
  for(int si = 0; si < SAMPLES; ++si) {
    uint8_t s = rs[ri][si];
    om |= s; // bits 0 if no samples have the button pressed
    am &= s; // bits 1 if all samples have the button pressed
  }
  // only change key state if all samples on or off
  rdb[ri] = (am | rdb[ri]) & om; 
}

static uint8_t kempstonJoystick = 0;

uint8_t __not_in_flash_func(pzx_kempston)() {
// 000FUDLR
#ifdef PICOMPUTER_PICOZX
// 000DRULF
  return kempstonJoystick 
  ? ((rdb[KEY_FIRE_ROW] & KEY_FIRE_BIT) << 4)    // Kempston fire
  | ((rdb[KEY_LEFT_ROW] & KEY_LEFT_BIT) )        // Kempston left
  | ((rdb[KEY_UP_ROW] & KEY_UP_BIT) << 1)        // Kempston up
  | ((rdb[KEY_RIGHT_ROW] & KEY_RIGHT_BIT) >> 3)  // Kempston right
  | ((rdb[KEY_DOWN_ROW] & KEY_DOWN_BIT) >> 2)    // Kempston down
  : 0;
#else
  return kempstonJoystick
  ? ((rdb[KEY_FIRE_ROW] & KEY_FIRE_BIT) >> 1)    // Kempston fire
  | ((rdb[KEY_UP_ROW] & KEY_UP_BIT) >> 2)        // Kempston up
  | ((rdb[KEY_DOWN_ROW] & KEY_DOWN_BIT) >> 3)    // Kempston down
  | ((rdb[KEY_LEFT_ROW] & KEY_LEFT_BIT) >> 4)    // Kempston left
  | ((rdb[KEY_RIGHT_ROW] & KEY_RIGHT_BIT) >> 5)  // Kempston right
  : 0;
#endif
}

void __not_in_flash_func(pzx_keyscan_get_hid_reports)(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev) {

  // Key modifier (shift, alt, ctrl, etc.)
  static uint8_t modifier = 0;

#if defined(PICOMPUTER_MAX) || defined(PICOMPUTER_ZX) || defined(PICOMPUTER_VGA)
  
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
      kbi = 2 + kempstonJoystick;
    }
    else if (rdb[KEY_LEFT_ROW] & KEY_LEFT_BIT) {
      // Numeric keyboard off
      kbi = 0 + kempstonJoystick;
    }
    if (rdb[3] & (1<<3)) {
      // Cursor mode
      kempstonJoystick = 0;
      kbi &= ~1;
    }
    else if (rdb[3] & (1<<4)) {
      // Joystick mode
      kempstonJoystick = 1;
      kbi |= 1;
    }    
    rdb[KEY_UP_ROW] &= ~KEY_UP_BIT;
    rdb[KEY_DOWN_ROW] &= ~KEY_DOWN_BIT;
    rdb[KEY_RIGHT_ROW] &= ~KEY_RIGHT_BIT;
    rdb[KEY_LEFT_ROW] &= ~KEY_LEFT_BIT;
    rdb[3] &= ~((1<<3) | (1<<4));
  }
#else
  bool shift = rdb[KEY_SHIFT_ROW] & KEY_SHIFT_BIT;
  #ifndef REAL_ZXKEYBOARD
  // Cursor mode 
  if (shift) {
    if (rdb[KEY_CURSOR_ROW] & KEY_CURSOR_BIT) { 
      kempstonJoystick = 0;
      gpio_put(LED_PIN, 0);
    }
    // Joystick mode
    if (rdb[KEY_KEMPSTON_ROW] & KEY_KEMPSTON_BIT) {
      kempstonJoystick = 2;
      gpio_put(LED_PIN, 1);
    }
  }
  #endif
  kbi = kempstonJoystick + (shift ? 1 : 0 );
#endif
  
  // Build the current hid report
  uint32_t ki = 0;
  hid_keyboard_report_t *chr = &hr[hri & 1];
  chr->modifier = modifier;
#if defined(PICOMPUTER_MAX) || defined(PICOMPUTER_ZX) || defined(PICOMPUTER_VGA)
  // Press ctrl for quick save
  if (alt_down && (((rdb[0] | rdb[1] | rdb[2]) & 31) != 0)) chr->modifier |= 1;
#else
  if (shift) chr->modifier |= 2;
#endif
  for(int ri = 0; ri < RN; ++ri) {
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
#if defined(PICOMPUTER_MAX) || defined(PICOMPUTER_ZX) || defined(PICOMPUTER_VGA)
        uint8_t kc = kbits[alt_down ? 4 : kbi][ri][ci];
#else
        uint8_t kc = kbits[kbi][ri][ci];
#endif
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
