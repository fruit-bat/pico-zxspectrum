#pragma once

typedef enum
{
    zx_spectrum_video_driver_vga_index = 0,
    zx_spectrum_video_driver_dvi_index,
    zx_spectrum_video_driver_lcd_index,
} zx_spectrum_video_driver_enum_t;

#define ZX_SPECTRUM_VIDEO_DRIVER_COUNT (zx_spectrum_video_driver_lcd_index + 1)
