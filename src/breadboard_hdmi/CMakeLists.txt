
add_executable(ZxSpectrumBreadboardHdmi
  ${zxspectrum_common_src}
  main.cpp
  tmds_encode_zxspectrum.S
  tmds_encode_zxspectrum.h
  hw_config.c
)

target_compile_definitions(ZxSpectrumBreadboardHdmi PRIVATE
  DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  PCS_COLS=80 
  PCS_ROWS=30  
)

target_link_libraries(ZxSpectrumBreadboardHdmi
  ${zxspectrum_common_libs}
  libdvi
  tinyusb_host
  tinyusb_board
  pico_emu_utils
  pico_dvi_menu
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumBreadboardHdmi)

