
set(picomputer_vgazx_defines
  ${picomputer_common_defines}
)

set(ZxSpectrumPicomputerVgaZx_640x480x60Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Set the VGA timing
  VGA_MODE=VideoVGA       
  # The display resolution
  DISPLAY_WIDTH_PIXELS=640
  DISPLAY_HEIGHT_PIXELS=480
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToCpu
)

set(ZxSpectrumPicomputerVgaZx_720x576x50Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=90
  PCS_ROWS=30
  SZ_FRAME_X=5
  # Set the VGA timing
  VGA_MODE=VideoDVD
  # The display resolution
  DISPLAY_WIDTH_PIXELS=720
  DISPLAY_HEIGHT_PIXELS=576
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToDisplay
)

foreach(target
  ZxSpectrumPicomputerVga1111Zx_640x480x60Hz
  ZxSpectrumPicomputerVga222Zx_640x480x60Hz
  ZxSpectrumPicomputerVgaAuk_640x480x60Hz
  ZxSpectrumPicomputerVgaAukBob_640x480x60Hz
  ZxSpectrumPicomputerVga1111Zx_720x576x50Hz
  ZxSpectrumPicomputerVga222Zx_720x576x50Hz
  ZxSpectrumPicomputerVgaAuk_720x576x50Hz
  ZxSpectrumPicomputerVgaAukBob_720x576x50Hz
)

  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_rgb_src}
    ${picomputer_common_src}
    ${zxspectrum_rgb332_src} 
    main.cpp
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    pico_emu_utils
    pico_vga_menu
    pico_vga_332
    tinyusb_host
    tinyusb_board
    hardware_pio
    hardware_pwm
  )

  pico_enable_stdio_usb(${target} 0)
  pico_enable_stdio_uart(${target} 0)

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

  
endforeach()

set(ZxSpectrumPicomputerVga1111Zx_defines
  ${picomputer_vgazx_defines}
  VGA_ENC_RGBY_1111
  VGA_GPIO_SYNC=4
  VGA_GPIO_OUTNUM=4
  SPK_PIN=9
  PICOMPUTER_VGA
)

target_compile_definitions(ZxSpectrumPicomputerVga1111Zx_640x480x60Hz PRIVATE
  ${ZxSpectrumPicomputerVga1111Zx_defines}
  ${ZxSpectrumPicomputerVgaZx_640x480x60Hz_defines}
)


target_compile_definitions(ZxSpectrumPicomputerVga1111Zx_720x576x50Hz PRIVATE
  ${ZxSpectrumPicomputerVga1111Zx_defines}
  ${ZxSpectrumPicomputerVgaZx_720x576x50Hz_defines}
)

set(ZxSpectrumPicomputerVga222Zx_defines
  ${picomputer_vgazx_defines}
  VGA_ENC_RGB_222
  VGA_GPIO_SYNC=6
  VGA_GPIO_OUTNUM=6
  SPK_PIN=7
  PICOMPUTER_PICOZX
)

target_compile_definitions(ZxSpectrumPicomputerVga222Zx_640x480x60Hz PRIVATE
  ${ZxSpectrumPicomputerVga222Zx_defines}
  ${ZxSpectrumPicomputerVgaZx_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumPicomputerVga222Zx_720x576x50Hz PRIVATE
  ${ZxSpectrumPicomputerVga222Zx_defines}
  ${ZxSpectrumPicomputerVgaZx_720x576x50Hz_defines}
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
set(ZxSpectrumPicomputerVgaAuk_defines
  ${picomputer_vgazx_defines}
  VGA_ENC_RGB_222
  VGA_GPIO_SYNC=6
  VGA_GPIO_OUTNUM=6
  SPK_PIN=7
  PICOMPUTER_PICOZX
  REAL_ZXKEYBOARD
)

target_compile_definitions(ZxSpectrumPicomputerVgaAuk_640x480x60Hz PRIVATE
  ${ZxSpectrumPicomputerVgaAuk_defines}
  ${ZxSpectrumPicomputerVgaZx_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumPicomputerVgaAuk_720x576x50Hz PRIVATE
  ${ZxSpectrumPicomputerVgaAuk_defines}
  ${ZxSpectrumPicomputerVgaZx_720x576x50Hz_defines}
)

#
# Target for bobricius
#
  set(ZxSpectrumPicomputerVgaAukBob_defines
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

target_compile_definitions(ZxSpectrumPicomputerVgaAukBob_640x480x60Hz PRIVATE
  ${ZxSpectrumPicomputerVgaAukBob_defines}
  ${ZxSpectrumPicomputerVgaZx_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumPicomputerVgaAukBob_720x576x50Hz PRIVATE
  ${ZxSpectrumPicomputerVgaAukBob_defines}  
  ${ZxSpectrumPicomputerVgaZx_720x576x50Hz_defines}
)
