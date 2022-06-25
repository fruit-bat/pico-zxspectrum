
add_executable(ZxSpectrumPicomputerVgaZx
  ${zxspectrum_common_src}
  ${picomputer_common_src}
  main.cpp
  pzx_prepare_vga332_scanline.cpp
)

target_compile_definitions(ZxSpectrumPicomputerVgaZx PRIVATE
  PCS_COLS=80 
  PCS_ROWS=30
  SPK_PIN=9
  PICOMPUTER_VGA
)

target_link_libraries(ZxSpectrumPicomputerVgaZx
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerVgaZx 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerVgaZx 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerVgaZx)

