#pragma once

#include "tusb.h"

void pzx_keyscan_init();
void __not_in_flash_func(pzx_keyscan_row)();
uint8_t __not_in_flash_func(pzx_kempston)();
void __not_in_flash_func(pzx_keyscan_get_hid_reports)(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev);

