set(zxspectrum_hdmi_src
  main.cpp
  tmds_encode_zxspectrum.S
  tmds_encode_zxspectrum.h
  audio_i2s.cpp
)

add_executable(ZxSpectrumPicoDv
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZxSpectrumPicoDv 
  ${CMAKE_CURRENT_LIST_DIR}/audio_i2s.pio
)

target_compile_definitions(ZxSpectrumPicoDv PRIVATE
  # Set up the DVI output to match the Pimoroni Pico DV board
  DVI_DEFAULT_SERIAL_CONFIG=pimoroni_demo_hdmi_cfg
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI cd card to use PIO
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

set(zxspectrum_hdmi_libs
  libdvi
  tinyusb_host
  tinyusb_board
  pico_emu_utils
  pico_dvi_menu
)

target_link_libraries(ZxSpectrumPicoDv
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicoDv)


