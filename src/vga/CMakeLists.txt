
########################################################################
# VGA 1111 on breadboard with PS/2 keyboard
########################################################################

set(picomputer_vga_defines
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
  # Use stdio
  USE_STDIO
)

set(zxspectrum_vga_640x480x60Hz_defines
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

set(zxspectrum_vga_720x576x50Hz_defines
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
  ZxSpectrum4PinAudioVga1111Ps2_640x480x60Hz
  ZxSpectrum4PinAudioVga1111Ps2_720x576x50Hz
  ZxSpectrum4PinAudioVga222Ps2_640x480x60Hz
  ZxSpectrum4PinAudioVga222Ps2_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_rgb_src}
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

  pico_generate_pio_header(${target}
    ${zxspectrum_ear_in_pio}
  )

  pico_enable_stdio_usb(${target} 0)
  pico_enable_stdio_uart(${target} 1)

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

endforeach()

set(ZxSpectrum4PinAudioVga1111Ps2_defines
  # VGA 1111
  VGA_ENC_RGBY_1111
  VGA_GPIO_FIRST=12
  VGA_GPIO_SYNC=16
  VGA_GPIO_OUTNUM=4
  # 4 pin audio
  BZR_PIN=21
  EAR_PIN=11
  AY8912_A_PIN=20
  AY8912_B_PIN=26
  AY8912_C_PIN=27
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrum4PinAudioVga1111Ps2_720x576x50Hz PRIVATE
  ${picomputer_vga_defines}
  ${ZxSpectrum4PinAudioVga1111Ps2_defines}
  ${zxspectrum_vga_720x576x50Hz_defines}
)

target_compile_definitions(ZxSpectrum4PinAudioVga1111Ps2_640x480x60Hz PRIVATE
  ${picomputer_vga_defines}
  ${ZxSpectrum4PinAudioVga1111Ps2_defines}
  ${zxspectrum_vga_640x480x60Hz_defines}
)

########################################################################
# VGA 222 on breadboard with PS/2 keyboard
########################################################################

set(ZxSpectrum4PinAudioVga222Ps2_defines
  ${picomputer_vga_defines}
  #VGA 222
  VGA_ENC_RGB_222
  VGA_GPIO_SYNC=18
  VGA_GPIO_OUTNUM=6
  # 4 pin audio
  BZR_PIN=21
  AY8912_A_PIN=20
  AY8912_B_PIN=26
  AY8912_C_PIN=27
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrum4PinAudioVga222Ps2_720x576x50Hz PRIVATE
  ${ZxSpectrum4PinAudioVga222Ps2_defines}
  ${zxspectrum_vga_720x576x50Hz_defines}
)

target_compile_definitions(ZxSpectrum4PinAudioVga222Ps2_640x480x60Hz PRIVATE
  ${ZxSpectrum4PinAudioVga222Ps2_defines}
  ${zxspectrum_vga_640x480x60Hz_defines}
)

#
# Inital attempt at PicoMiteVGA support:-
#
# Stuff to do:
#
# 1) May need to move out from here
# 2) May need code modifications for joystick handling
# 3) PIN definitions not updated
# 6) RGBY is probably the wrong order
# 7) c-sync vs h-sync and v-sync
# 8) May be better off basing this on the Murulator build
#
set(ZxSpectrumPicoMiteVGA_defines
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=10
  SDCARD_PIN_SPI0_SCK=12
  SDCARD_PIN_SPI0_MOSI=11
  SDCARD_PIN_SPI0_MISO=13
  # VGA 1111
  VGA_ENC_BGYR_1111
  VGA_GPIO_FIRST=18
  VGA_GPIO_SYNC=16
  VGA_GPIO_OUTNUM=4
  # 4 pin audio
  BZR_PIN=6
  SPK_PIN=7
  # Enable the PS/2 keyboard
  USE_PS2_KBD
  USE_MRMLTR_PS2_KBD
  # Use stdio
  USE_STDIO
)

foreach(target

  ZxSpectrumPicoMiteVGA_640x480x60Hz
  ZxSpectrumPicoMiteVGA_720x576x50Hz
  ZX-MURMULATOR_VGA_640x480x60Hz
  ZX-MURMULATOR_VGA_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_rgb_src}
    ${murmulator_common_src}
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

  pico_generate_pio_header(${target}
    ${zxspectrum_ear_in_pio}
  )

  pico_generate_pio_header(${target}
    ${murmulator_ps2kbd_pio}
  )

  pico_enable_stdio_usb(${target} 0)

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

endforeach()

foreach(target
  ZxSpectrumPicoMiteVGA_640x480x60Hz
  ZxSpectrumPicoMiteVGA_720x576x50Hz
)
  pico_enable_stdio_uart(${target} 1)

endforeach()

foreach(target
  ZX-MURMULATOR_VGA_640x480x60Hz
  ZX-MURMULATOR_VGA_720x576x50Hz
)
  pico_enable_stdio_uart(${target} 0)

endforeach()

target_compile_definitions(ZxSpectrumPicoMiteVGA_720x576x50Hz PRIVATE
  ${ZxSpectrumPicoMiteVGA_defines}
  ${zxspectrum_vga_720x576x50Hz_defines}
)

target_compile_definitions(ZxSpectrumPicoMiteVGA_640x480x60Hz PRIVATE
  ${ZxSpectrumPicoMiteVGA_defines}
  ${zxspectrum_vga_640x480x60Hz_defines}
)

#========================================================
# ZX MURMULATOR
#========================================================
set(ZX-MURMULATOR_VGA_defines
  MURMULATOR
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
  #VGA 222
  VGA_ENC_RGB_222
  VGA_GPIO_FIRST=6
  VGA_GPIO_OUTNUM=6
  VGA_GPIO_SYNC=12
  VGA_VSYNC=13
  # Speaker pin audio
  SPK_PIN=28
  # Buzzer audio out
  # BZR_PIN=28
  # STEREO AY audio
  AY8912_ABC_STEREO
  # Left chanel audio out
  AY8912_A_PIN=27
  # Right chanel audio out
  AY8912_C_PIN=26
  # Use the alternate volume map
  AY8912_ALT_VOL_MAP
  # Tape audio in
  EAR_PIN=22
  # Enable the PS/2 keyboard
  USE_PS2_KBD
  USE_MRMLTR_PS2_KBD
)

target_compile_definitions(ZX-MURMULATOR_VGA_720x576x50Hz PRIVATE
  ${ZX-MURMULATOR_VGA_defines}
  ${zxspectrum_vga_720x576x50Hz_defines}
)

target_compile_definitions(ZX-MURMULATOR_VGA_640x480x60Hz PRIVATE
  ${ZX-MURMULATOR_VGA_defines}
  ${zxspectrum_vga_640x480x60Hz_defines}
)
