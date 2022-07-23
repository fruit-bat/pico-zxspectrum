set(zxspectrum_hdmi_src
  main.cpp
  tmds_encode_zxspectrum.S
  tmds_encode_zxspectrum.h
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

target_compile_definitions(ZxSpectrumBreadboardHdmi1PinAudio PRIVATE
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  PCS_COLS=80
  PCS_ROWS=30
  SPK_PIN=20
)

target_compile_definitions(ZxSpectrumBreadboardHdmi2PinAudio PRIVATE
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  PCS_COLS=80
  PCS_ROWS=30
  SPK_PIN=20
  BZR_PIN=21
)

target_compile_definitions(ZxSpectrumBreadboardHdmi4PinAudio PRIVATE
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  PCS_COLS=80
  PCS_ROWS=30
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

