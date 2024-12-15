
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
  ${zxspectrum_scanvideo_src}
  main.cpp
  st7789_lcd.cpp
  pzx_prepare_rgb444_scanline.cpp
)

set(zxspectrum_zx_lcd_640x480x60Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Set the VGA timing
  VGA_MODE=vga_mode_640x240_60
  # The display resolution
  DISPLAY_WIDTH_PIXELS=640
  DISPLAY_HEIGHT_PIXELS=480
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToCpu
  # TODO work what this is
  PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=700
)

set(zxspectrum_zx_lcd_720x576x50Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=90
  PCS_ROWS=30
  SZ_FRAME_X=5
  # Set the VGA timing
  VGA_MODE=vga_mode_720x288_50
  # The display resolution
  DISPLAY_WIDTH_PIXELS=720
  DISPLAY_HEIGHT_PIXELS=576
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToDisplay
  # TODO work what this is
  PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=800
)

set(picomputer_zx_lcd_common_defines PRIVATE
  ${picomputer_common_defines}
  PICO_PIO_PWM_AUDIO  
  PICO_AUDIO_PWM_PIO=pio1
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
  PICO_SCANVIDEO_COLOR_PIN_COUNT=6u
  PICO_SCANVIDEO_DPI_PIXEL_RCOUNT=2u
  PICO_SCANVIDEO_DPI_PIXEL_GCOUNT=2u
  PICO_SCANVIDEO_DPI_PIXEL_BCOUNT=3u
  PICO_SCANVIDEO_DPI_PIXEL_RSHIFT=4u
  PICO_SCANVIDEO_DPI_PIXEL_GSHIFT=2u
  PICO_SCANVIDEO_DPI_PIXEL_BSHIFT=0u
  PICO_SCANVIDEO_COLOR_PIN_BASE=0u
)

set(zxspectrum_zx_lcd_common_libs
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_st7789_menu
  pico_scanvideo_dpi
  pico_scanvideo_menu
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

foreach(target
  ZxSpectrumPicomputerZxLcd_640x480x60Hz
  ZxSpectrumPicomputerZxLcd_720x576x50Hz
  ZxSpectrumPicomputerZxInverseLcd_640x480x60Hz
  ZxSpectrumPicomputerZxInverseLcd_720x576x50Hz
  ZxSpectrumPicomputerZxILI9341Lcd_720x576x50Hz
  ZxSpectrumPicomputerZxILI9341Lcd_640x480x60Hz
)
  add_executable(${target}
    ${picomputer_zx_lcd_src}
  )

  pico_generate_pio_header(${target}
    ${CMAKE_CURRENT_LIST_DIR}/st7789_lcd.pio
  )

  pico_generate_pio_header(${target} 
    ${zxspectrum_pwm_pio}
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
  # Speaker pin audio (@270MHz 270000000 / (1000 * 3 * 2)) 
  PICO_PWM_AUDIO_FREQ=45000 
)

target_compile_definitions(ZxSpectrumPicomputerZxLcd_640x480x60Hz PRIVATE
  ${picomputer_zx_lcd_common_defines}
  ${zxspectrum_zx_lcd_640x480x60Hz_defines}
  LCD_MIRROR_X
  # Speaker pin audio (@250MHz 250000000 / (1000 * 3 * 2)) 
  PICO_PWM_AUDIO_FREQ=41666  
)

target_compile_definitions(ZxSpectrumPicomputerZxInverseLcd_720x576x50Hz PRIVATE
  ${picomputer_zx_lcd_common_defines}
  ${zxspectrum_zx_lcd_720x576x50Hz_defines}
  LCD_MIRROR_X
  LCD_INVERSE
  # Speaker pin audio (@270MHz 270000000 / (1000 * 3 * 2)) 
  PICO_PWM_AUDIO_FREQ=45000   
)

target_compile_definitions(ZxSpectrumPicomputerZxInverseLcd_640x480x60Hz PRIVATE
  ${picomputer_zx_lcd_common_defines}
  ${zxspectrum_zx_lcd_640x480x60Hz_defines}
  LCD_MIRROR_X
  LCD_INVERSE
  # Speaker pin audio (@250MHz 250000000 / (1000 * 3 * 2)) 
  PICO_PWM_AUDIO_FREQ=41666  
)

target_compile_definitions(ZxSpectrumPicomputerZxILI9341Lcd_720x576x50Hz PRIVATE
  ${picomputer_zx_lcd_common_defines}
  ${zxspectrum_zx_lcd_720x576x50Hz_defines}
  LCD_INVERSE
  LCD_RGB
  # Speaker pin audio (@270MHz 270000000 / (1000 * 3 * 2)) 
  PICO_PWM_AUDIO_FREQ=45000   
)

target_compile_definitions(ZxSpectrumPicomputerZxILI9341Lcd_640x480x60Hz PRIVATE
  ${picomputer_zx_lcd_common_defines}
  ${zxspectrum_zx_lcd_640x480x60Hz_defines}
  LCD_INVERSE
  LCD_RGB
  # Speaker pin audio (@250MHz 250000000 / (1000 * 3 * 2)) 
  PICO_PWM_AUDIO_FREQ=41666  
)
