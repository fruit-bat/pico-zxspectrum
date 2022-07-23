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
  # Configure the SPI cd card to use SPI0
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4  
)

target_compile_definitions(ZxSpectrumBreadboardHdmi1PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  SPK_PIN=20
)

target_compile_definitions(ZxSpectrumBreadboardHdmi2PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  SPK_PIN=20
  BZR_PIN=21
)

target_compile_definitions(ZxSpectrumBreadboardHdmi4PinAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  BZR_PIN=21
  AY8912_A_PIN=20
  AY8912_B_PIN=26
  AY8912_C_PIN=27
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

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi1PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi2PinAudio)
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi4PinAudio)

