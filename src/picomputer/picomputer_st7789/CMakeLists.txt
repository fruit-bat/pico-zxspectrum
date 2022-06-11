
include_directories(${picomputer_common_inc})

add_executable(ZxSpectrumPicomputerMax
  ${zxspectrum_common_src}
  ${picomputer_common_src}
	main.cpp
  st7789_lcd.cpp
  pzx_prepare_rgb444_scanline.cpp
)

pico_generate_pio_header(ZxSpectrumPicomputerMax 
  ${CMAKE_CURRENT_LIST_DIR}/st7789_lcd.pio
)

target_compile_definitions(ZxSpectrumPicomputerMax PRIVATE
  PCS_COLS=40 
  PCS_ROWS=30
  PICOMPUTER_MAX
)

target_link_libraries(ZxSpectrumPicomputerMax
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_st7789_menu
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerMax 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerMax 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerMax)

add_executable(ZxSpectrumPicomputerZX
  ${zxspectrum_common_src}
  ${picomputer_common_src}
	main.cpp
  st7789_lcd.cpp
  pzx_prepare_rgb444_scanline.cpp
)

pico_generate_pio_header(ZxSpectrumPicomputerZX 
  ${CMAKE_CURRENT_LIST_DIR}/st7789_lcd.pio
)

target_compile_definitions(ZxSpectrumPicomputerZX PRIVATE
  PCS_COLS=40 
  PCS_ROWS=30
  PICOMPUTER_ZX
  SZ_TITLE_MARGIN=3
)

target_link_libraries(ZxSpectrumPicomputerZX
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_st7789_menu
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicomputerZX 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerZX 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerZX)
