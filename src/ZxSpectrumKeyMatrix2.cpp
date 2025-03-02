#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "class/hid/hid.h"
#include "ZxSpectrumKeyMatrix.h"

#define SAMPLES 2

#define ROW_PINS 9,1,8,5,4,15
#define COL_PIN_DAT 10
#define COL_PIN_CLK 11
// TODO
#define ROW_PINS_JOIN(a) (a)
#define ROW_PINS_SHIFT 2
#define COL_PIN_COUNT 8
// TODO Check this works when not a power of 2
#define ROW_PIN_COUNT 6

static uint8_t row_pins[] = {ROW_PINS};                // Row pins
static uint8_t rs[COL_PIN_COUNT][SAMPLES];       // Oversampled pins
static uint8_t rdb[COL_PIN_COUNT];               // Debounced pins
static hid_keyboard_report_t hr[2];              // Current and previous hid report
static uint8_t hri = 0;                          // Currenct hid report index
static uint8_t kbi = 0;

static uint8_t kbits[1][ROW_PIN_COUNT][COL_PIN_COUNT] = {
  {
    // TODO Rotate etc
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

// TODO Wrong
#define KEY_SHIFT_ROW 0
#define KEY_SHIFT_BIT 0x20
#define KEY_CTRL_ROW 5
#define KEY_CTRL_BIT 0x01
#define LED_PIN 25

void zx_keyscan_init() {

   gpio_init(COL_PIN_DAT);
   gpio_set_dir(COL_PIN_DAT, GPIO_OUT);
   gpio_init(COL_PIN_CLK);
   gpio_set_dir(COL_PIN_CLK, GPIO_OUT);
   gpio_put(COL_PIN_DAT, 1);
   gpio_put(COL_PIN_CLK, 0);
   sleep_ms(1);

   for(int i = 0; i < COL_PIN_COUNT; ++i) {
     gpio_put(COL_PIN_CLK, 1);
     sleep_ms(1);
     gpio_put(COL_PIN_CLK, 0);
     sleep_ms(1);
   }

   gpio_put(COL_PIN_DAT, 0);
   sleep_ms(1);
   gpio_put(COL_PIN_CLK, 1);
   sleep_ms(1);

   for(int i = 0; i < ROW_PIN_COUNT; ++i) {
      gpio_init(row_pins[i]);
      gpio_set_dir(row_pins[i], GPIO_IN);
      gpio_pull_up(row_pins[i]);
   }
}

void __not_in_flash_func(zx_keyscan_row)() {
  static uint32_t ri = 0;
  static uint32_t si = 0;

  gpio_put(COL_PIN_CLK, 0);

  uint32_t a = ~(gpio_get_all() >> ROW_PINS_SHIFT);

  uint32_t r = ROW_PINS_JOIN(a);
  rs[ri][si] = (uint8_t)r;
  
  
  if (++ri >= COL_PIN_COUNT) {
    ri = 0;
    if (++si >= SAMPLES) si = 0;
  }

  gpio_put(COL_PIN_DAT, ri == 0 ? 0 : 1);
  
  uint32_t om = 0;
  uint32_t am = (1 << ROW_PIN_COUNT) - 1;
  for(int si = 0; si < SAMPLES; ++si) {
    uint8_t s = rs[ri][si];
    om |= s; // bits 0 if no samples have the button pressed
    am &= s; // bits 1 if all samples have the button pressed
  }
  // only change key state if all samples on or off
  rdb[ri] = (am | rdb[ri]) & om; 
  
  gpio_put(COL_PIN_CLK, 1);
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
  for(int ri = 0; ri < COL_PIN_COUNT; ++ri) {
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

