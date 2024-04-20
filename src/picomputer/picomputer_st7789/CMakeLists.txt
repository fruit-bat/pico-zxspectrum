
include_directories(${picomputer_common_inc})

add_executable(ZxSpectrumPicomputerMax
  ${zxspectrum_common_src}
  ${picomputer_common_src}
  ${zxspectrum_rgb332_src}
  main.cpp
  st7789_lcd.cpp
  pzx_prepare_rgb444_scanline.cpp
)

pico_generate_pio_header(ZxSpectrumPicomputerMax 
  ${CMAKE_CURRENT_LIST_DIR}/st7789_lcd.pio
)

target_compile_definitions(ZxSpectrumPicomputerMax PRIVATE
  ${picomputer_common_defines}
  # Set the resolution of the menu screen
  PCS_COLS=40 
  PCS_ROWS=30
  # Set the size of the menus
  SZ_WIZ_ML=0
  SZ_WIZ_CW1=12
  SZ_WIZ_CW2=18
  SZ_FRAME_COLS=40
  SZ_FRAME_ROWS=30
  SZ_FRAME_X=0
  SZ_FRAME_Y=0
  SZ_MENU_SEP=2
  SZ_FILE_SEP=1
  SZ_FILE_ROWS=18
  SPK_PIN=0
  PICOMPUTER_MAX
  LCD_MIRROR_X
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
  ${picomputer_common_defines}
  # Set the resolution of the menu screen
  PCS_COLS=40 
  PCS_ROWS=30
  # Set the size of the menus
  SZ_WIZ_ML=0
  SZ_WIZ_CW1=12
  SZ_WIZ_CW2=18
  SZ_FRAME_COLS=34
  SZ_FRAME_ROWS=23
  SZ_FRAME_X=3
  SZ_FRAME_Y=3
  SZ_MENU_SEP=1
  SZ_FILE_SEP=1
  SZ_FILE_ROWS=11
  SPK_PIN=0
  PICOMPUTER_ZX
  LCD_MIRROR_X
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

set(picomputer_zx_lcd_src
  ${zxspectrum_common_src}
  ${picomputer_common_src}
  ${zxspectrum_rgb_src}
  ${zxspectrum_rgb332_src} 
  main.cpp
  st7789_lcd.cpp
  pzx_prepare_rgb444_scanline.cpp
)

set(zxspectrum_zx_lcd_640x480x60Hz_defines
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

set(zxspectrum_zx_lcd_720x576x50Hz_defines
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


set(picomputer_zx_lcd_common_defines PRIVATE
  ${picomputer_common_defines}
  SPK_PIN=7
  PICOMPUTER_PICOZX
  PICOMPUTER_PICOZX_LCD
  INITIAL_VOL=0x30
  LCD_NO_BL
  LCD_PIN_CLK=2
  LCD_PIN_DC=6
  LCD_PIN_CS=5
  LCD_PIN_DIN=3
  VGA_ENC_RGB_222
  VGA_GPIO_SYNC=6
  VGA_GPIO_OUTNUM=6
)

set(zxspectrum_zx_lcd_common_libs
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_st7789_menu
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

foreach(target
  ZxSpectrumPicomputerZxLcd_640x480x60Hz
  ZxSpectrumPicomputerZxLcd_720x576x50Hz
)
  add_executable(${target}
    ${picomputer_zx_lcd_src}
  )
  
  pico_generate_pio_header(${target} 
    ${CMAKE_CURRENT_LIST_DIR}/st7789_lcd.pio
  )

  target_link_libraries(${target}
    ${zxspectrum_zx_lcd_common_libs}
  )


  pico_enable_stdio_usb(${target} 0)
  pico_enable_stdio_uart(${target} 0)
  
  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

endforeach()

target_compile_definitions(ZxSpectrumPicomputerZxLcd_720x576x50Hz PRIVATE
  ${picomputer_zx_lcd_common_defines}
  ${zxspectrum_zx_lcd_720x576x50Hz_defines}
  LCD_MIRROR_X
)
  
target_compile_definitions(ZxSpectrumPicomputerZxLcd_640x480x60Hz PRIVATE
  ${picomputer_zx_lcd_common_defines}
  ${zxspectrum_zx_lcd_640x480x60Hz_defines}
  LCD_MIRROR_X
)

add_executable(ZxSpectrumPicomputerZxInverseLcd
  ${picomputer_zx_lcd_src}
)

pico_generate_pio_header(ZxSpectrumPicomputerZxInverseLcd 
  ${CMAKE_CURRENT_LIST_DIR}/st7789_lcd.pio
)

target_compile_definitions(ZxSpectrumPicomputerZxInverseLcd PRIVATE
  ${picomputer_zx_lcd_common_defines}
  LCD_MIRROR_X
  LCD_INVERSE
)

target_link_libraries(ZxSpectrumPicomputerZxInverseLcd
  ${zxspectrum_zx_lcd_common_libs}
)

pico_enable_stdio_usb(ZxSpectrumPicomputerZxInverseLcd 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerZxInverseLcd 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerZxInverseLcd)


add_executable(ZxSpectrumPicomputerZxILI9341Lcd
  ${picomputer_zx_lcd_src}
)

pico_generate_pio_header(ZxSpectrumPicomputerZxILI9341Lcd 
  ${CMAKE_CURRENT_LIST_DIR}/st7789_lcd.pio
)

target_compile_definitions(ZxSpectrumPicomputerZxILI9341Lcd PRIVATE
  ${picomputer_zx_lcd_common_defines}
  LCD_INVERSE
  LCD_RGB
)

target_link_libraries(ZxSpectrumPicomputerZxILI9341Lcd
  ${zxspectrum_zx_lcd_common_libs}
)

pico_enable_stdio_usb(ZxSpectrumPicomputerZxILI9341Lcd 0)
pico_enable_stdio_uart(ZxSpectrumPicomputerZxILI9341Lcd 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerZxILI9341Lcd)