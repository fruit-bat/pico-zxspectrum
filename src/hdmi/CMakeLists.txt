########################################################################
# Breadboard HDMI
########################################################################

set(zxspectrum_hdmi_src
  main.cpp
  ${zxspectrum_dvi_src}
)

add_executable(ZxSpectrumBreadboardHdmiAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

add_executable(ZxSpectrumBreadboardHdmiAudio720x540x50Hz
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZxSpectrumBreadboardHdmiAudio 
  ${zxspectrum_ear_in_pio}
)

add_executable(ZxSpectrumBreadboardHdmi1PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZxSpectrumBreadboardHdmi1PinAudio 
  ${zxspectrum_ear_in_pio}
)

add_executable(ZxSpectrumBreadboardHdmi2PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZxSpectrumBreadboardHdmi2PinAudio 
  ${zxspectrum_ear_in_pio}
)

add_executable(ZxSpectrumBreadboardHdmi4PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZxSpectrumBreadboardHdmi4PinAudio 
  ${zxspectrum_ear_in_pio}
)

add_executable(ZxSpectrumBreadboardHdmiKbd1PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
  ${zxspectrum_kbd_src}
)

set(zxspectrum_hdmi_defines
  # Set up the DVI output to match the Pimoroni Pico DV board
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  PICO_CORE1_STACK_SIZE=0x400
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Tell FATFS we want to find files
  FF_USE_FIND=1  
  # Configure the SPI sd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
)

set(zxspectrum_hdmi_640x480x60Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Set the DVI timing
  DVI_TIMING=dvi_timing_640x480p_60hz
  # The display resolution
  DISPLAY_WIDTH_PIXELS=640
  DISPLAY_HEIGHT_PIXELS=480
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToCpu
)

set(zxspectrum_hdmi_720x540x50Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=90
  PCS_ROWS=30
  # Set the DVI timing
  DVI_TIMING=dvi_timing_720x540p_50hz
  # The display resolution
  DISPLAY_WIDTH_PIXELS=720
  DISPLAY_HEIGHT_PIXELS=540
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToDisplay
)

target_compile_definitions(ZxSpectrumBreadboardHdmiAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  EAR_PIN=11
  PICO_HDMI_AUDIO 
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmiAudio720x540x50Hz PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_720x540x50Hz_defines}
#  EAR_PIN=11
  PICO_HDMI_AUDIO 
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmi1PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  SPK_PIN=20
  EAR_PIN=11
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmi2PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  SPK_PIN=20
  EAR_PIN=11
  BZR_PIN=21
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmi4PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  BZR_PIN=21
  EAR_PIN=11
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

target_link_libraries(ZxSpectrumBreadboardHdmiAudio
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
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
pico_add_extra_outputs(ZxSpectrumBreadboardHdmiAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi1PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi2PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi4PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmiKbd1PinAudio)

########################################################################
# Pico DV
########################################################################

set (zxspectrum_pico_dv_defines
  # Set up the DVI output to match the Pimoroni Pico DV board
  DVI_DEFAULT_SERIAL_CONFIG=pimoroni_demo_hdmi_cfg
  PICO_CORE1_STACK_SIZE=0x400
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use PIO
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=22
  SDCARD_PIN_SPI0_SCK=5
  SDCARD_PIN_SPI0_MOSI=18
  SDCARD_PIN_SPI0_MISO=19
)

add_executable(ZxSpectrumPicoDv_I2SAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

add_executable(ZxSpectrumPicoDv_HdmiAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZxSpectrumPicoDv_I2SAudio 
  ${zxspectrum_audio_i2s_pio}
)

pico_generate_pio_header(ZxSpectrumPicoDv_HdmiAudio 
  ${zxspectrum_audio_i2s_pio}
)

target_compile_definitions(ZxSpectrumPicoDv_I2SAudio PRIVATE
  ${zxspectrum_pico_dv_defines}
  # Configure the I2S audio 
  PICO_AUDIO_I2S=1
  PICO_AUDIO_I2S_SM=2
  PICO_AUDIO_I2S_PIO=pio1
  PICO_AUDIO_I2S_PIO_FUNC=GPIO_FUNC_PIO1
  PICO_AUDIO_I2S_DATA=26
  PICO_AUDIO_I2S_BCLK=27
)

target_compile_definitions(ZxSpectrumPicoDv_HdmiAudio PRIVATE
  ${zxspectrum_pico_dv_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  PICO_HDMI_AUDIO
)

target_link_libraries(ZxSpectrumPicoDv_I2SAudio
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

target_link_libraries(ZxSpectrumPicoDv_HdmiAudio
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicoDv_I2SAudio)
pico_add_extra_outputs(ZxSpectrumPicoDv_HdmiAudio)

########################################################################
# Waveshare - RP2040 - PicoZero
########################################################################

add_executable(ZxSpectrumPiZero
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

target_compile_definitions(ZxSpectrumPiZero PRIVATE
# Set up the DVI output to match the Waveshare RP2040 PiZero
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  DVI_CUSTOM_CONFIG
  DVI_DEFAULT_SERIAL_CONFIG=waveshare_rp2040_pizero_hdmi_cfg
  PICO_CORE1_STACK_SIZE=0x400
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use PIO
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=21
  SDCARD_PIN_SPI0_SCK=18
  SDCARD_PIN_SPI0_MOSI=19
  SDCARD_PIN_SPI0_MISO=20
  # HDMI audio 
  PICO_HDMI_AUDIO
)

set(zxspectrum_hdmi_libs
  libdvi
  tinyusb_host
  tinyusb_board
  pico_emu_utils
  pico_dvi_menu
)

target_link_libraries(ZxSpectrumPiZero
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPiZero)




