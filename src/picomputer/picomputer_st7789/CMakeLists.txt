
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
  SZ_WIZ_ML=0
  SZ_WIZ_CW1=12
  SZ_WIZ_CW2=18
  SZ_FRAME_COLS=40
  SZ_FRAME_ROWS=30
  SZ_FRAME_X=0
  SZ_FRAME_Y=0
  SZ_MENU_SEP=2
  SZ_FILE_ROWS=9
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
  SZ_WIZ_ML=0
  SZ_WIZ_CW1=12
  SZ_WIZ_CW2=18
  SZ_FRAME_COLS=34
  SZ_FRAME_ROWS=23
  SZ_FRAME_X=3
  SZ_FRAME_Y=3
  SZ_MENU_SEP=1
  SZ_FILE_ROWS=11    
  PICOMPUTER_ZX
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

