
########################################################################
# VGA on Pimoroni Pico VGA Demo Base
# PICO_BOARD=vgaboard
# cmake -DPICO_COPY_TO_RAM=0 -DPICO_BOARD=vgaboard ..
########################################################################

set(ZxSpectrumPicoVga_defines
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use PIO
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=22
  SDCARD_PIN_SPI0_SCK=5
  SDCARD_PIN_SPI0_MOSI=18
  SDCARD_PIN_SPI0_MISO=19

  # Configure the I2S audio 
  PICO_AUDIO_I2S=1
  PICO_AUDIO_I2S_SM=2
  PICO_AUDIO_I2S_PIO=pio1
  PICO_AUDIO_I2S_PIO_FUNC=GPIO_FUNC_PIO1
  PICO_AUDIO_I2S_DATA=26
  PICO_AUDIO_I2S_BCLK=27  
)

set(zxspectrum_vga_640x480x60Hz_defines
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

set(zxspectrum_picovga_720x576x50Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=90
  PCS_ROWS=30
  SZ_FRAME_X=5
  # Set the VGA timing
  VGA_MODE=vga_mode_768x288_50  
  # The display resolution
  DISPLAY_WIDTH_PIXELS=720
  DISPLAY_HEIGHT_PIXELS=576
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToDisplay
  # TODO work what this is
  PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=800
)

foreach(target 
  ZxSpectrumPicoVga_640x480x60Hz
  ZxSpectrumPicoVga_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_scanvideo_src}
    main.cpp
  )

  pico_generate_pio_header(${target} 
    ${zxspectrum_audio_i2s_pio}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    pico_emu_utils
    pico_scanvideo_menu
    tinyusb_host
    tinyusb_board
    hardware_pio
    hardware_pwm
    pico_scanvideo_dpi
  )

  pico_enable_stdio_usb(${target} 0)
  pico_enable_stdio_uart(${target} 0)

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})
endforeach()

target_compile_definitions(ZxSpectrumPicoVga_640x480x60Hz PRIVATE
 ${ZxSpectrumPicoVga_defines}
 ${zxspectrum_vga_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumPicoVga_720x576x50Hz PRIVATE
  ${ZxSpectrumPicoVga_defines}
  ${zxspectrum_picovga_720x576x50Hz_defines}
)
