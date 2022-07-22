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
  DVI_DEFAULT_SERIAL_CONFIG=pimoroni_demo_hdmi_cfg
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  PCS_COLS=80
  PCS_ROWS=30
  FF_USE_FIND=1
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=22
  SDCARD_PIN_SPI0_SCK=5
  SDCARD_PIN_SPI0_MOSI=18
  SDCARD_PIN_SPI0_MISO=19
  AUDIO_I2S=1

#  # compile time configuration of I2S
#  PICO_AUDIO_I2S_MONO_INPUT=1
#  #define for our example code
#  USE_AUDIO_I2S=1
#  PICO_AUDIO_I2S_DMA_IRQ=1
#  PICO_AUDIO_I2S_PIO=1
#  PICO_AUDIO_I2S_DATA=26
#  PICO_AUDIO_I2S_BCLK=27  
)

set(zxspectrum_hdmi_libs
  libdvi
  tinyusb_host
  tinyusb_board
  pico_emu_utils
  pico_dvi_menu
# pico_audio_i2s
)

target_link_libraries(ZxSpectrumPicoDv
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicoDv)


