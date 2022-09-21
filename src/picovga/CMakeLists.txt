
add_executable(ZxSpectrumPicoVga
  ${zxspectrum_common_src}
  main.cpp
)

pico_generate_pio_header(ZxSpectrumPicoVga 
  ${zxspectrum_audio_i2s_pio}
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
  pico_vga16_menu
  tinyusb_host
  tinyusb_board
  hardware_pio
  hardware_pwm
  pico_scanvideo_dpi
)

pico_enable_stdio_usb(ZxSpectrumPicoVga 0)
pico_enable_stdio_uart(ZxSpectrumPicoVga 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicoVga)



