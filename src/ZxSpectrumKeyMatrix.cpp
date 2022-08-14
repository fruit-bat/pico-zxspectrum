#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "class/hid/hid.h"
#include "ZxSpectrumKeyMatrix.h"

#define SAMPLES 4 

#define CP 9, 10, 11, 21, 22, 26, 27, 28
#define RP 0, 1, 6, 7, 8
#define CP_JOIN(a) ((a & 7) | ((a >> 9) & (3 << 3)) | ((a >> 12) & (7 << 5)))
#define CP_SHIFT 9
#define RN 5
#define CN 8

static uint8_t cp[] = {CP};                      // Column pins
static uint8_t rp[] = {RP};                      // Row pins
static uint8_t rs[RN][SAMPLES];                  // Oversampled pins
static uint8_t rdb[RN];                          // Debounced pins
static hid_keyboard_report_t hr[2];              // Current and previous hid report
static uint8_t hri = 0;                          // Currenct hid report index
static uint8_t kbi = 0;

static uint8_t kbits[1][5][8] = { 
  {
    {HID_KEY_1, HID_KEY_Q, HID_KEY_A, HID_KEY_0, HID_KEY_P, 0 /* SHIFT */, HID_KEY_ENTER, HID_KEY_SPACE},
    {HID_KEY_2, HID_KEY_W, HID_KEY_S, HID_KEY_9, HID_KEY_O, HID_KEY_Z,     HID_KEY_L,     HID_KEY_ALT_RIGHT},
    {HID_KEY_3, HID_KEY_E, HID_KEY_D, HID_KEY_8, HID_KEY_I, HID_KEY_X,     HID_KEY_K,     HID_KEY_M},
    {HID_KEY_4, HID_KEY_R, HID_KEY_F, HID_KEY_7, HID_KEY_U, HID_KEY_C,     HID_KEY_J,     HID_KEY_N},
    {HID_KEY_5, HID_KEY_T, HID_KEY_G, HID_KEY_6, HID_KEY_Y, HID_KEY_V,     HID_KEY_H,     HID_KEY_B}
  }
};


#define KEY_SHIFT_ROW 0
#define KEY_SHIFT_BIT 0x20
#define LED_PIN 25

void zx_keyscan_init() {

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

void __not_in_flash_func(zx_keyscan_row)() {
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

void __not_in_flash_func(zx_keyscan_get_hid_reports)(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev) {

  // Key modifier (shift, alt, ctrl, etc.)
  static uint8_t modifier = 0;
  
  bool shift = rdb[KEY_SHIFT_ROW] & KEY_SHIFT_BIT;

  // Build the current hid report
  uint32_t ki = 0;
  hid_keyboard_report_t *chr = &hr[hri & 1];
  chr->modifier = modifier;
  if (shift) chr->modifier |= 2;
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

