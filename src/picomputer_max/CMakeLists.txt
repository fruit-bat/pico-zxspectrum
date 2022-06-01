
add_executable(ZxSpectrumPicomputerMax
  ${zxspectrum_common_src}
#	main.cpp
  hello_world.cpp
#  hw_config.c
#  ZxSpectrumPicomputerVgaJoystick.cpp
#  pzx_keyscan.cpp  
)

target_compile_definitions(ZxSpectrumPicomputerMax PRIVATE
  -DPCS_COLS=80 
  -DPCS_ROWS=30
)

target_link_libraries(ZxSpectrumPicomputerMax
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  
#  tinyusb_host
#  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerMax 1)
pico_enable_stdio_uart(ZxSpectrumPicomputerMax 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerMax)

