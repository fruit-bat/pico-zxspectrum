########################################################################
# ZX-MURMULATOR
########################################################################

set(zxspectrum_hdmi_src
  main.cpp
  ${zxspectrum_dvi_src}
  ${murmulator_common_src}
)

add_executable(ZX-MURMULATOR_HDMI
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
)

pico_generate_pio_header(ZX-MURMULATOR_HDMI
  ${zxspectrum_ear_in_pio}
)

pico_generate_pio_header(ZX-MURMULATOR_HDMI
  ${murmulator_ps2kbd_pio}
)


# ============================================================
target_compile_definitions(ZX-MURMULATOR_HDMI PRIVATE
  # Set up the DVI output to match the MURMULATOR board
  MURMULATOR
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
  #SDCARD_PIO=pio1
  #SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
  # Speaker pin audio
  SPK_PIN=28
  # Buzzer audio out
  # BZR_PIN=28
  # STEREO AY audio
  AY8912_ABC_STEREO
  # Left chanel audio out
  AY8912_A_PIN=27
  # Right chanel audio out
  AY8912_C_PIN=26
  # Use the alternate volume map
  AY8912_ALT_VOL_MAP  
  # Tape audio in
  EAR_PIN=22
  # Enable the PS/2 keyboard
  USE_PS2_KBD
  
)

set(zxspectrum_hdmi_libs
  libdvi
  tinyusb_host
  tinyusb_board
  pico_emu_utils
  pico_dvi_menu
)

target_link_libraries(ZX-MURMULATOR_HDMI
  ${zxspectrum_common_libs}
  ${zxspectrum_hdmi_libs}
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZX-MURMULATOR_HDMI)




