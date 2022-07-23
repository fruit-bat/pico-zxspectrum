
add_executable(ZxSpectrumPicomputerVga
  ${zxspectrum_common_src}
  ${zxspectrum_rgb_src}  
  ${picomputer_common_src}
  main.cpp
)

target_compile_definitions(ZxSpectrumPicomputerVga PRIVATE
  ${picomputer_common_defines}
  PCS_COLS=80 
  PCS_ROWS=30
  SPK_PIN=9
  VGA_ENC_RGB_332
  PICOMPUTER_VGA
)

target_link_libraries(ZxSpectrumPicomputerVga
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerVga 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerVga 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerVga)

