#include "ZxSpectrumHidMouse.h"
#include "hid_host_info.h"

void __not_in_flash_func(ZxSpectrumHidMouse::setButtons)(uint32_t b)
{
    // Buttons	D0:Right D1:Left D2:Middle
    _buttons = ~(
        (b & MOUSE_BUTTON_LEFT ? 2 : 0) | 
        (b & MOUSE_BUTTON_MIDDLE ? 4 : 0) |
        (b & MOUSE_BUTTON_RIGHT ? 1 : 0)
    );
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::buttons)()
{
    return _buttons;
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::xAxis)()
{
    return _xAcc >> 3;
}

uint8_t __not_in_flash_func(ZxSpectrumHidMouse::yAxis)()
{
    return _yAcc >> 3;
}