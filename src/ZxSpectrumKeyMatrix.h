#pragma once

#include "tusb.h"

void zx_keyscan_init();
void zx_menu_mode(bool m);
void zx_keyscan_row();
void zx_keyscan_get_hid_reports(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev);
bool zx_fire_raw();
uint8_t zx_kempston();
