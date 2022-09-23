
set(picomputer_vga_defines
  PCS_COLS=80 
  PCS_ROWS=30
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
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


