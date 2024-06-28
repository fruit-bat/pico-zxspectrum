#include "ZxSpectrumHidMouse.h"

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::buttons)() {
    return (~_buttons) & 0xff;
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::xAxis)() {
    return _xAcc >> 2;
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::yAxis)() {
    return _yAcc >> 2;
}