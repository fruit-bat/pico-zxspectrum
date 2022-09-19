
add_executable(ZxSpectrumPicoVga
  ${zxspectrum_common_src}
  main.cpp
)

target_compile_definitions(ZxSpectrumPicoVga PRIVATE
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use PIO
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=22
  SDCARD_PIN_SPI0_SCK=5
  SDCARD_PIN_SPI0_MOSI=18
  SDCARD_PIN_SPI0_MISO=19
  # Set up the menu 80 columns and 30 rows
  PCS_COLS=80 
  PCS_ROWS=30
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
  DEBUG_FAT_SPI
  
  # VGA 16
  # TODO

  # Configure the I2S audio 
  PICO_AUDIO_I2S=1
  PICO_AUDIO_I2S_SM=2
  PICO_AUDIO_I2S_PIO=pio1
  PICO_AUDIO_I2S_PIO_FUNC=GPIO_FUNC_PIO1
  PICO_AUDIO_I2S_DATA=26
  PICO_AUDIO_I2S_BCLK=27  
  
)

target_link_libraries(ZxSpectrumPicoVga
  ${zxspectrum_common_libs}
  pico_emu_utils
  pico_vga_menu
  pico_vga_332
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
)

pico_enable_stdio_usb(ZxSpectrumPicoVga 0)
pico_enable_stdio_uart(ZxSpectrumPicoVga 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicoVga)



