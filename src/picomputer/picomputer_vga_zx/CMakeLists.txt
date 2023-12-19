
set(picomputer_vgazx_defines
  ${picomputer_common_defines}
  PCS_COLS=80 
  PCS_ROWS=30
)

add_executable(ZxSpectrumPicomputerVga1111Zx
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  ${picomputer_common_src}
  main.cpp
)

target_compile_definitions(ZxSpectrumPicomputerVga1111Zx PRIVATE
  ${picomputer_vgazx_defines}
  VGA_ENC_RGBY_1111
  VGA_GPIO_SYNC=4
  VGA_GPIO_OUTNUM=4
  SPK_PIN=9
  PICOMPUTER_VGA
)

target_link_libraries(ZxSpectrumPicomputerVga1111Zx
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerVga1111Zx 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerVga1111Zx 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerVga1111Zx)


add_executable(ZxSpectrumPicomputerVga222Zx
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  ${picomputer_common_src}
  main.cpp
)

target_compile_definitions(ZxSpectrumPicomputerVga222Zx PRIVATE
  ${picomputer_vgazx_defines}
  VGA_ENC_RGB_222
  VGA_GPIO_SYNC=6
  VGA_GPIO_OUTNUM=6
  SPK_PIN=7
  PICOMPUTER_PICOZX
)

target_link_libraries(ZxSpectrumPicomputerVga222Zx
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerVga222Zx 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerVga222Zx 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerVga222Zx)

add_executable(ZxSpectrumPicomputerVgaAuk
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  ${picomputer_common_src}
  main.cpp
)

#
# AUK Hardware + Compatible board from bobricius
# ----------------------------------------------
#
# AUK extra keyboard key assignments:
# 
# F = Fire
# U = Up
# D = Down
# L = Left
# R = Right
# N = Next snap
# P = Previous snap
# A = 48K
# B = 128k
# Q = Quick save 1
# C = Joystick cursor mode
# K = Joystick kempton mode
#
#                   Normal    Shifted
# Column:           12345678  12345678
# AUK original:     FLURDMAB  FLURDMAB
# AUK digitalmexer: MFLURDAB  MFLURDAB
# AUK bobrisius:    FLURDMPN  FLURDQCK
#
target_compile_definitions(ZxSpectrumPicomputerVgaAuk PRIVATE
  ${picomputer_vgazx_defines}
  VGA_ENC_RGB_222
  VGA_GPIO_SYNC=6
  VGA_GPIO_OUTNUM=6
  SPK_PIN=7
  PICOMPUTER_PICOZX
  REAL_ZXKEYBOARD
)

target_link_libraries(ZxSpectrumPicomputerVgaAuk
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerVgaAuk 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerVgaAuk 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerVgaAuk)

#
# Target for bobricius
#
add_executable(ZxSpectrumPicomputerVgaAukBob
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  ${picomputer_common_src}
  main.cpp
)

target_compile_definitions(ZxSpectrumPicomputerVgaAukBob PRIVATE
  ${picomputer_vgazx_defines}
  VGA_ENC_RGB_222
  VGA_GPIO_SYNC=6
  VGA_GPIO_OUTNUM=6
  SPK_PIN=7
  PICOMPUTER_PICOZX
  REAL_ZXKEYBOARD
  REAL_ZXKEYBOARD_BOB
  HID_KEY_MOD_SAVE=2
)

target_link_libraries(ZxSpectrumPicomputerVgaAukBob
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerVgaAukBob 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerVgaAukBob 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerVgaAukBob)
