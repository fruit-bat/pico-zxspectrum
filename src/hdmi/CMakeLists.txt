########################################################################
# Breadboard HDMI
########################################################################

set(zxspectrum_hdmi_src
  main.cpp
  ${zxspectrum_dvi_src}
)

add_executable(ZxSpectrumBreadboardHdmi1PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

add_executable(ZxSpectrumBreadboardHdmi2PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

add_executable(ZxSpectrumBreadboardHdmi4PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

add_executable(ZxSpectrumBreadboardHdmiKbd1PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
  ${zxspectrum_kbd_src}
)

set(zxspectrum_hdmi_defines
  # Set up the DVI output to match the Pimoroni Pico DV board
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Tell FATFS we want to find files
  FF_USE_FIND=1  
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
)

target_compile_definitions(ZxSpectrumBreadboardHdmi1PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  SPK_PIN=20
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmi2PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  SPK_PIN=20
  BZR_PIN=21
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmi4PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  BZR_PIN=21
  AY8912_A_PIN=20
  AY8912_B_PIN=26
  AY8912_C_PIN=27
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmiKbd1PinAudio PRIVATE
  # Set up the DVI output to match the Pimoroni Pico DV board
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Tell FATFS we want to find files
  FF_USE_FIND=1  
  # Single pin audio for now
  SPK_PIN=20
  # 21 is also free
  # Configure the SPI sd card to use PIO
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=28
  SDCARD_PIN_SPI0_SCK=22
  SDCARD_PIN_SPI0_MOSI=26
  SDCARD_PIN_SPI0_MISO=27  
  USE_KEY_MATRIX
)

set(zxspectrum_hdmi_libs
  libdvi
  tinyusb_host
  tinyusb_board
  pico_emu_utils
  pico_dvi_menu
)

target_link_libraries(ZxSpectrumBreadboardHdmi1PinAudio
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

target_link_libraries(ZxSpectrumBreadboardHdmi2PinAudio
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

target_link_libraries(ZxSpectrumBreadboardHdmi4PinAudio
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

target_link_libraries(ZxSpectrumBreadboardHdmiKbd1PinAudio
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

pico_enable_stdio_usb(ZxSpectrumBreadboardHdmiKbd1PinAudio 0)
pico_enable_stdio_uart(ZxSpectrumBreadboardHdmiKbd1PinAudio 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi1PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi2PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi4PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmiKbd1PinAudio)

########################################################################
# Pico DV
########################################################################

set(zxspectrum_hdmi_src
  main.cpp
  ${zxspectrum_dvi_src}
)

add_executable(ZxSpectrumPicoDv
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZxSpectrumPicoDv 
  ${zxspectrum_audio_i2s_pio}
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



