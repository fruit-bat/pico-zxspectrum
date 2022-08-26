#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "class/hid/hid.h"
#include "ZxSpectrumKeyMatrix.h"

#define SAMPLES 4 

#define CP 2, 3, 4, 5, 6, 7, 8, 9
#define RP_DAT 10
#define RP_CLK 11
#define CP_JOIN(a) (a)
#define CP_SHIFT 2
#define RN 8
#define CN 8

static uint8_t cp[] = {CP};                      // Column pins
static uint8_t rs[RN][SAMPLES];                  // Oversampled pins
static uint8_t rdb[RN];                          // Debounced pins
static hid_keyboard_report_t hr[2];              // Current and previous hid report
static uint8_t hri = 0;                          // Currenct hid report index
static uint8_t kbi = 0;

static uint8_t kbits[1][RN][CN] = {
  {
    // 48k Spectrum keys
    {HID_KEY_1, HID_KEY_Q, HID_KEY_A, HID_KEY_0, HID_KEY_P, 0 /* SHIFT */, HID_KEY_ENTER, HID_KEY_SPACE},
    {HID_KEY_2, HID_KEY_W, HID_KEY_S, HID_KEY_9, HID_KEY_O, HID_KEY_Z,     HID_KEY_L,     HID_KEY_ALT_RIGHT},
    {HID_KEY_3, HID_KEY_E, HID_KEY_D, HID_KEY_8, HID_KEY_I, HID_KEY_X,     HID_KEY_K,     HID_KEY_M},
    {HID_KEY_4, HID_KEY_R, HID_KEY_F, HID_KEY_7, HID_KEY_U, HID_KEY_C,     HID_KEY_J,     HID_KEY_N},
    {HID_KEY_5, HID_KEY_T, HID_KEY_G, HID_KEY_6, HID_KEY_Y, HID_KEY_V,     HID_KEY_H,     HID_KEY_B},
    // Extra keys - these are just some example mappings
    {0 /* CTRL */, HID_KEY_ESCAPE, HID_KEY_BACKSPACE, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_APOSTROPHE, HID_KEY_MINUS},
    {HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_ARROW_RIGHT, HID_KEY_ARROW_DOWN, HID_KEY_EQUAL, HID_KEY_SEMICOLON, HID_KEY_BRACKET_RIGHT, HID_KEY_BRACKET_LEFT},
    {HID_KEY_GRAVE, HID_KEY_BACKSLASH, HID_KEY_F1, HID_KEY_F2, HID_KEY_F3, HID_KEY_F4, HID_KEY_F5, HID_KEY_F6}
  }
};

#define KEY_SHIFT_ROW 0
#define KEY_SHIFT_BIT 0x20
#define KEY_CTRL_ROW 5
#define KEY_CTRL_BIT 0x01
#define LED_PIN 25

void zx_keyscan_init() {

   gpio_init(RP_DAT);
   gpio_set_dir(RP_DAT, GPIO_OUT);
   gpio_init(RP_CLK);
   gpio_set_dir(RP_CLK, GPIO_OUT);
   gpio_put(RP_DAT, 1);
   gpio_put(RP_CLK, 0);
   sleep_ms(1);

   for(int i = 0; i < RN; ++i) {
     gpio_put(RP_CLK, 1);
     sleep_ms(1);
     gpio_put(RP_CLK, 0);
     sleep_ms(1);
   }

   gpio_put(RP_DAT, 0);
   sleep_ms(1);
   gpio_put(RP_CLK, 1);
   sleep_ms(1);

   for(int i = 0; i < CN; ++i) {
      gpio_init(cp[i]);
      gpio_set_dir(cp[i], GPIO_IN);
      gpio_pull_up(cp[i]);
   }
}

void __not_in_flash_func(zx_keyscan_row)() {
  static uint32_t ri = 0;
  static uint32_t si = 0;

  gpio_put(RP_CLK, 0);

  uint32_t a = ~(gpio_get_all() >> CP_SHIFT);

  uint32_t r = CP_JOIN(a);
  rs[ri][si] = (uint8_t)r;
  
  
  if (++ri >= RN) {
    ri = 0;
    if (++si >= SAMPLES) si = 0;
  }

  gpio_put(RP_DAT, ri == 0 ? 0 : 1);
  
  uint32_t om = 0;
  uint32_t am = (1 << CN) - 1;
  for(int si = 0; si < SAMPLES; ++si) {
    uint8_t s = rs[ri][si];
    om |= s; // bits 0 if no samples have the button pressed
    am &= s; // bits 1 if all samples have the button pressed
  }
  // only change key state if all samples on or off
  rdb[ri] = (am | rdb[ri]) & om; 
  
  gpio_put(RP_CLK, 1);
}

void __not_in_flash_func(zx_keyscan_get_hid_reports)(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev) {

  // Key modifier (shift, alt, ctrl, etc.)
  static uint8_t modifier = 0;
  
  bool shift = rdb[KEY_SHIFT_ROW] & KEY_SHIFT_BIT;
  bool ctrl = rdb[KEY_CTRL_ROW] & KEY_CTRL_BIT;

  // Build the current hid report
  uint32_t ki = 0;
  hid_keyboard_report_t *chr = &hr[hri & 1];
  chr->modifier = modifier;
  if (shift) chr->modifier |= 2;
  if (ctrl) chr->modifier |= 1;
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
        uint8_t kc = kbits[kbi][ri][ci];
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

