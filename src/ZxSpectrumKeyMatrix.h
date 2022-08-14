#pragma once

#include "tusb.h"

void zx_keyscan_init();
void __not_in_flash_func(zx_keyscan_row)();
void __not_in_flash_func(zx_keyscan_get_hid_reports)(hid_keyboard_report_t const **curr, hid_keyboard_report_t const **prev);
