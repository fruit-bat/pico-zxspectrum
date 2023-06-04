
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
  # Tape audio in
  EAR_PIN=22
  # Enable the PS/2 keyboard
  USE_PS2_KBD    
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