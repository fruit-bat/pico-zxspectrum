
set(picomputer_vga_defines
  PCS_COLS=80 
  PCS_ROWS=30
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
  # Use stdio
  USE_STDIO
)

add_executable(ZxSpectrum4PinAudioVga1111Ps2
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  main.cpp
)

target_compile_definitions(ZxSpectrum4PinAudioVga1111Ps2 PRIVATE
  ${picomputer_vga_defines}
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

target_link_libraries(ZxSpectrum4PinAudioVga1111Ps2
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_generate_pio_header(ZxSpectrum4PinAudioVga1111Ps2 
  ${zxspectrum_ear_in_pio}
)

pico_enable_stdio_usb(ZxSpectrum4PinAudioVga1111Ps2 0)
pico_enable_stdio_uart(ZxSpectrum4PinAudioVga1111Ps2 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrum4PinAudioVga1111Ps2)


add_executable(ZxSpectrum4PinAudioVga222Ps2
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  ${picomputer_common_src}
  main.cpp
)

target_compile_definitions(ZxSpectrum4PinAudioVga222Ps2 PRIVATE
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

target_link_libraries(ZxSpectrum4PinAudioVga222Ps2
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrum4PinAudioVga222Ps2 0)
pico_enable_stdio_uart(ZxSpectrum4PinAudioVga222Ps2 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrum4PinAudioVga222Ps2)

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
add_executable(ZxSpectrumPicoMiteVGA
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  ${murmulator_common_src}
  main.cpp
)

target_compile_definitions(ZxSpectrumPicoMiteVGA PRIVATE
  PCS_COLS=80 
  PCS_ROWS=30
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

target_link_libraries(ZxSpectrumPicoMiteVGA
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_generate_pio_header(ZxSpectrumPicoMiteVGA
  ${murmulator_ps2kbd_pio}
)

pico_enable_stdio_usb(ZxSpectrumPicoMiteVGA 0)
pico_enable_stdio_uart(ZxSpectrumPicoMiteVGA 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicoMiteVGA)


#========================================================
# ZX MURMULATOR 
#========================================================
add_executable(ZX-MURMULATOR_VGA
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}
  ${murmulator_common_src}
  main.cpp
)

target_compile_definitions(ZX-MURMULATOR_VGA PRIVATE
  MURMULATOR
  PCS_COLS=80 
  PCS_ROWS=30
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

target_link_libraries(ZX-MURMULATOR_VGA
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_generate_pio_header(ZX-MURMULATOR_VGA
  ${zxspectrum_ear_in_pio}
)

pico_generate_pio_header(ZX-MURMULATOR_VGA
  ${murmulator_ps2kbd_pio}
)

pico_enable_stdio_usb(ZX-MURMULATOR_VGA 0)
pico_enable_stdio_uart(ZX-MURMULATOR_VGA 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZX-MURMULATOR_VGA)
