
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


