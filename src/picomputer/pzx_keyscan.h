#pragma once

#include "tusb.h"

void zx_keyscan_init();
void zx_keyscan_row();
uint8_t zx_kempston();
void zx_keyscan_get_hid_reports(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev);
void zx_menu_mode(bool menu);
bool zx_menu_mode();
bool zx_fire_raw();

