########################################################################
# Breadboard HDMI
########################################################################

set(zxspectrum_hdmi_src
  main.cpp
  ${zxspectrum_dvi_src}
)

foreach(target
  ZxSpectrumBreadboardHdmiAudio
  ZxSpectrumBreadboardHdmiAudio720x540x50Hz
  ZxSpectrumBreadboardHdmiAudio720x576x50Hz
  ZxSpectrumBreadboardHdmi1PinAudio
  ZxSpectrumBreadboardHdmi2PinAudio
  ZxSpectrumBreadboardHdmi4PinAudio
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_hdmi_src}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_ear_in_pio}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_pwm_pio}
  )

endforeach()

add_executable(ZxSpectrumBreadboardHdmiKbd1PinAudio
  ${zxspectrum_common_src}
  ${zxspectrum_hdmi_src}
  ${zxspectrum_kbd_src}
)

set(zxspectrum_hdmi_defines
  DVI_DEFAULT_SERIAL_CONFIG=pico_sock_cfg
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
  # Speaker pin audio (@252MHz 252000000 / (1000 * 3 * 2))
  PICO_PWM_AUDIO_FREQ=42000
)

set(zxspectrum_hdmi_720x540x50Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=90
  PCS_ROWS=30
  SZ_FRAME_X=5
  # Set the DVI timing
  DVI_TIMING=dvi_timing_720x540p_50hz
  # The display resolution
  DISPLAY_WIDTH_PIXELS=720
  DISPLAY_HEIGHT_PIXELS=540
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToDisplay
  # Speaker pin audio (@270MHz 270000000 / (1000 * 3 * 2))
  PICO_PWM_AUDIO_FREQ=45000
)

set(zxspectrum_hdmi_720x576x50Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=90
  PCS_ROWS=30
  SZ_FRAME_X=5
  # Set the DVI timing
  DVI_TIMING=dvi_timing_720x576p_50hz
  # The display resolution
  DISPLAY_WIDTH_PIXELS=720
  DISPLAY_HEIGHT_PIXELS=576
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToDisplay
  # Speaker pin audio (@270MHz 270000000 / (1000 * 3 * 2))
  PICO_PWM_AUDIO_FREQ=45000
)

set(zxspectrum_hdmi_nespad_defines
  # Pins for dual NES joypads
  NESPAD_DATA_PIN_BASE=20
  NESPAD_CLK_PIN_BASE=26
  NESPAD_PIO=pio1
  NESPAD_IRQ=PIO1_IRQ_0
  NESPAD_ENABLE
)

target_compile_definitions(ZxSpectrumBreadboardHdmiAudio PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  ${zxspectrum_hdmi_nespad_defines}
  EAR_PIN=11
  PICO_HDMI_AUDIO
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmiAudio720x540x50Hz PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_720x540x50Hz_defines}
  ${zxspectrum_hdmi_nespad_defines}
  EAR_PIN=11
  PICO_HDMI_AUDIO
  # Enable the PS/2 keyboard
  USE_PS2_KBD
)

target_compile_definitions(ZxSpectrumBreadboardHdmiAudio720x576x50Hz PRIVATE
  ${zxspectrum_hdmi_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
#  ${zxspectrum_hdmi_nespad_defines}
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

foreach(target
  ZxSpectrumBreadboardHdmiAudio
  ZxSpectrumBreadboardHdmiAudio720x540x50Hz
  ZxSpectrumBreadboardHdmiAudio720x576x50Hz
  ZxSpectrumBreadboardHdmi1PinAudio
  ZxSpectrumBreadboardHdmi2PinAudio
  ZxSpectrumBreadboardHdmi4PinAudio
  ZxSpectrumBreadboardHdmiKbd1PinAudio
)
  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})
endforeach()

pico_enable_stdio_usb(ZxSpectrumBreadboardHdmiKbd1PinAudio 0)
pico_enable_stdio_uart(ZxSpectrumBreadboardHdmiKbd1PinAudio 0)

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

set (zxspectrum_pico_dv_i2saudio_defines
  # Configure the I2S audio
  PICO_AUDIO_I2S=1
  PICO_AUDIO_I2S_PIO=pio1
  PICO_AUDIO_I2S_PIO_FUNC=GPIO_FUNC_PIO1
  PICO_AUDIO_I2S_DATA=26
  PICO_AUDIO_I2S_BCLK=27
)

foreach(target
  ZxSpectrumPicoDv_I2SAudio_640x480x60Hz
  ZxSpectrumPicoDv_HdmiAudio_640x480x60Hz
  ZxSpectrumPicoDv_I2SAudio_720x576x50Hz
  ZxSpectrumPicoDv_HdmiAudio_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_hdmi_src}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_audio_i2s_pio}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  pico_add_extra_outputs(${target})

endforeach()

target_compile_definitions(ZxSpectrumPicoDv_I2SAudio_640x480x60Hz PRIVATE
  ${zxspectrum_pico_dv_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  ${zxspectrum_pico_dv_i2saudio_defines}
)

target_compile_definitions(ZxSpectrumPicoDv_HdmiAudio_640x480x60Hz PRIVATE
  ${zxspectrum_pico_dv_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  PICO_HDMI_AUDIO
)

target_compile_definitions(ZxSpectrumPicoDv_I2SAudio_720x576x50Hz PRIVATE
  ${zxspectrum_pico_dv_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  ${zxspectrum_pico_dv_i2saudio_defines}
)

target_compile_definitions(ZxSpectrumPicoDv_HdmiAudio_720x576x50Hz PRIVATE
  ${zxspectrum_pico_dv_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  PICO_HDMI_AUDIO
)

########################################################################
# Adafruit Feather DVI
########################################################################

# cmake -DPICO_COPY_TO_RAM=0   -DPICO_SDK_PATH=../../pico-sdk -DPICO_BOARD=adafruit_feather_rp2040 ..
# make -j4 ZxSpectrumAdafruitFeatherHdmi
foreach(target
  ZxSpectrumAdafruitFeatherHdmi
)

  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_hdmi_src}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})
endforeach()

set(zx_spectrum_adafuit_feather_hdmi_defines
  DVI_DEFAULT_SERIAL_CONFIG=adafruit_feather_dvi_cfg
  PICO_CORE1_STACK_SIZE=0x400
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use SPI0
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=13
  SDCARD_PIN_SPI0_SCK=10
  SDCARD_PIN_SPI0_MOSI=11
  SDCARD_PIN_SPI0_MISO=12

  PICO_HDMI_AUDIO
  
  # defines from the board type
#  PICO_BOOT_STAGE2_CHOOSE_GENERIC_03H=1

#  PICO_FLASH_SPI_CLKDIV=4
#  PICO_RP2040_B0_SUPPORTED=0
)

target_compile_definitions(ZxSpectrumAdafruitFeatherHdmi PRIVATE
  ${zx_spectrum_adafuit_feather_hdmi_defines}
  # ${zxspectrum_hdmi_720x576x50Hz_defines} - this seems to be very unstable
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  # PICO_HDMI_AUDIO - this results in a black screen
)

########################################################################
# Waveshare - RP2040 - PicoZero
########################################################################

foreach(target
  ZxSpectrumPiZero_640x480x60Hz
  ZxSpectrumPiZero_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_hdmi_src}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

endforeach()

set (ZxSpectrumPiZero_defines
  # Set up the DVI output to match the Waveshare RP2040 PiZero
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

target_compile_definitions(ZxSpectrumPiZero_640x480x60Hz PRIVATE
  ${ZxSpectrumPiZero_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumPiZero_720x576x50Hz PRIVATE
  ${ZxSpectrumPiZero_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
)

########################################################################
# Waveshare - RP2040 - PicoZero - For Bobricius PICO ZX board
########################################################################

foreach(target
  ZxSpectrumBobZero_640x480x60Hz
  ZxSpectrumBobZero_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_st7789_lcd_src}
    ${zxspectrum_hdmi_src}
    ${zxspectrum_kbd2_src}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_st7789_pio}
  )

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

endforeach()


set (ZxSpectrumBobZero_defines
  # Set up the DVI output to match the Waveshare RP2040 PiZero
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
  # Use the matrix keyboard
  USE_KEY_MATRIX
)

target_compile_definitions(ZxSpectrumBobZero_640x480x60Hz PRIVATE
  ${ZxSpectrumPiZero_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumBobZero_720x576x50Hz PRIVATE
  ${ZxSpectrumPiZero_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
)

########################################################################
# ZX-MURMULATOR
########################################################################

set(zxspectrum_murmulator_src
  main.cpp
  ${zxspectrum_dvi_src}
)

set(zxspectrum_murmulator_hdmi_nespad_defines
  # Pins for dual NES joypads
  NESPAD_DATA_PIN_BASE=16
  NESPAD_CLK_PIN_BASE=14
  NESPAD_PIO=pio1
  NESPAD_IRQ=PIO1_IRQ_0
  NESPAD_ENABLE
)

set(murmulator_common_defines
  ZX_PLATFORM="MURMULATOR"
  # Set up the DVI output to match the MURMULATOR board
  PICO_CORE1_STACK_SIZE=0x400
  DVI_DEFAULT_SERIAL_CONFIG=pimoroni_demo_hdmi_cfg
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use PIO
  #SDCARD_PIO=pio1
  #SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=2
  SDCARD_PIN_SPI0_MOSI=3
  SDCARD_PIN_SPI0_MISO=4
  # Use the alternate volume map
  AY8912_ALT_VOL_MAP
  # Tape audio in
  EAR_PIN=22
  # Enable the PS/2 keyboard
  USE_PS2_KBD
  PS2KBD_CLK0_DAT
)

set(murmulator_pwm_audio_defines
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
)

set(murmulator_hdmi_audio_defines
  # Use HDMI Audio
  PICO_HDMI_AUDIO
)

foreach(target
  ZX-MURMULATOR_HDMI_PWM_AUDIO_640x480x60Hz
  ZX-MURMULATOR_HDMI_PWM_AUDIO_720x576x50Hz
  ZX-MURMULATOR_HDMI_HDMI_AUDIO_640x480x60Hz
  ZX-MURMULATOR_HDMI_HDMI_AUDIO_720x576x50Hz
  ZX-MURMULATOR_HDMI_I2S_AUDIO_640x480x60Hz
  ZX-MURMULATOR_HDMI_I2S_AUDIO_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_murmulator_src}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_ear_in_pio}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_audio_i2s_pio}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_pwm_pio}
  )

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

endforeach()

target_compile_definitions(ZX-MURMULATOR_HDMI_HDMI_AUDIO_640x480x60Hz PRIVATE
  ${murmulator_common_defines}
  ${murmulator_hdmi_audio_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  ${zxspectrum_murmulator_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR_HDMI_HDMI_AUDIO_720x576x50Hz PRIVATE
  ${murmulator_common_defines}
  ${murmulator_hdmi_audio_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  ${zxspectrum_murmulator_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR_HDMI_PWM_AUDIO_640x480x60Hz PRIVATE
  ${murmulator_common_defines}
  ${murmulator_pwm_audio_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  ${zxspectrum_murmulator_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR_HDMI_PWM_AUDIO_720x576x50Hz PRIVATE
  ${murmulator_common_defines}
  ${murmulator_pwm_audio_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  ${zxspectrum_murmulator_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR_HDMI_I2S_AUDIO_640x480x60Hz PRIVATE
  ${murmulator_common_defines}
  ${zxspectrum_pico_dv_i2saudio_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  ${zxspectrum_murmulator_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR_HDMI_I2S_AUDIO_720x576x50Hz PRIVATE
  ${murmulator_common_defines}
  ${zxspectrum_pico_dv_i2saudio_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  ${zxspectrum_murmulator_hdmi_nespad_defines}
)

########################################################################
# ZX-MURMULATOR2
########################################################################

set(zxspectrum_murmulator2_src
  main.cpp
  ${zxspectrum_dvi_src}
)

set(zxspectrum_murmulator2_hdmi_nespad_defines
  # Pins for dual NES joypads
  NESPAD_DATA_PIN_BASE=26
  NESPAD_CLK_PIN_BASE=20
  NESPAD_PIO=pio1
  NESPAD_IRQ=PIO1_IRQ_0
  NESPAD_ENABLE
)

set(murmulator2_common_defines
  ZX_PLATFORM="MURMULATOR2"
  # Set up the DVI output to match the MURMULATOR board
  DVI_CUSTOM_CONFIG
  DVI_DEFAULT_SERIAL_CONFIG=murmulator2_hdmi_cfg
  PICO_CORE1_STACK_SIZE=0x400
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use PIO
  SDCARD_PIN_SPI0_CS=5
  SDCARD_PIN_SPI0_SCK=6
  SDCARD_PIN_SPI0_MOSI=7
  SDCARD_PIN_SPI0_MISO=4
  # Use the alternate volume map
  AY8912_ALT_VOL_MAP
  # Tape audio in
  EAR_PIN=22
  # Enable the PS/2 keyboard
  USE_PS2_KBD
  PS2KBD_CLK2_DAT
)

set(murmulator2_pwm_audio_defines
  #
  # This could be ABC output on GPIO 10, 9, 11 but the sofware FIFO is not working on the RP2350
  # For now I will output mono PWM using a PIO block on GPIO 10
  #
  PICO_AUDIO_PWM_PIO=pio1
  PICO_PIO_PWM_AUDIO
  SPK_PIN=9
)

set (zxspectrum_murmulator2_i2saudio_defines
  # Configure the I2S audio
  PICO_AUDIO_I2S=1
  PICO_AUDIO_I2S_PIO=pio1
  PICO_AUDIO_I2S_PIO_FUNC=GPIO_FUNC_PIO1
  PICO_AUDIO_I2S_DATA=9
  PICO_AUDIO_I2S_BCLK=10
)

set(murmulator2_hdmi_audio_defines
  # Use HDMI Audio
  PICO_HDMI_AUDIO
)

foreach(target
  ZX-MURMULATOR2_HDMI_PWM_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_HDMI_PWM_AUDIO_720x576x50Hz
  ZX-MURMULATOR2_HDMI_HDMI_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_HDMI_HDMI_AUDIO_720x576x50Hz
  ZX-MURMULATOR2_HDMI_I2S_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_HDMI_I2S_AUDIO_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_murmulator2_src}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_ear_in_pio}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_audio_i2s_pio}
  )

  pico_generate_pio_header(${target}
    ${zxspectrum_pwm_pio}
  )

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

  pico_enable_stdio_usb(${target} 0)
  pico_enable_stdio_uart(${target} 1)

endforeach()

target_compile_definitions(ZX-MURMULATOR2_HDMI_HDMI_AUDIO_640x480x60Hz PRIVATE
  ${murmulator2_common_defines}
  ${murmulator2_hdmi_audio_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  ${zxspectrum_murmulator2_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR2_HDMI_HDMI_AUDIO_720x576x50Hz PRIVATE
  ${murmulator2_common_defines}
  ${murmulator2_hdmi_audio_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  ${zxspectrum_murmulator2_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR2_HDMI_PWM_AUDIO_640x480x60Hz PRIVATE
  ${murmulator2_common_defines}
  ${murmulator2_pwm_audio_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
  ${zxspectrum_murmulator2_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR2_HDMI_PWM_AUDIO_720x576x50Hz PRIVATE
  ${murmulator2_common_defines}
  ${murmulator2_pwm_audio_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  ${zxspectrum_murmulator2_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR2_HDMI_I2S_AUDIO_640x480x60Hz PRIVATE
  ${murmulator2_common_defines}
  ${zxspectrum_murmulator2_i2saudio_defines}
  ${zxspectrum_hdmi2_640x480x60Hz_defines}
  ${zxspectrum_murmulator2_hdmi_nespad_defines}
)

target_compile_definitions(ZX-MURMULATOR2_HDMI_I2S_AUDIO_720x576x50Hz PRIVATE
  ${murmulator2_common_defines}
  ${zxspectrum_murmulator2_i2saudio_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
  ${zxspectrum_murmulator2_hdmi_nespad_defines}
)

########################################################################
# Olimex RP2040-PICO-PC
########################################################################

set(zxspectrum_olimex_pc_src
  main.cpp
  ${zxspectrum_dvi_src}
)

set(olimex_pc_common_defines
  # Set up the DVI output to match the olimex_pc board
  PICO_CORE1_STACK_SIZE=0x400
  DVI_DEFAULT_SERIAL_CONFIG=Olimex_RP2040_PICO_PC_cfg
  DVI_VERTICAL_REPEAT=2
  DVI_N_TMDS_BUFFERS=3
  DVI_1BPP_BIT_REVERSE=1
  # Tell FATFS we want to find files
  FF_USE_FIND=1
  # Configure the SPI sd card to use PIO
  #SDCARD_PIO=pio1
  #SDCARD_PIO_SM=1
  SDCARD_PIN_SPI0_CS=22
  SDCARD_PIN_SPI0_SCK=6
  SDCARD_PIN_SPI0_MOSI=7
  SDCARD_PIN_SPI0_MISO=4
)

set(olimex_pc_hdmi_audio_defines
  # Use HDMI Audio
  PICO_HDMI_AUDIO
)

foreach(target
  ZxSpectrumOlimexPcHdmiAudio640x480x60Hz
  ZxSpectrumOlimexPcHdmiAudio720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_olimex_pc_src}
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    ${zxspectrum_hdmi_libs}
  )

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

  pico_enable_stdio_usb(${target} 0)
  pico_enable_stdio_uart(${target} 1)

endforeach()

target_compile_definitions(ZxSpectrumOlimexPcHdmiAudio640x480x60Hz PRIVATE
  ${olimex_pc_common_defines}
  ${olimex_pc_hdmi_audio_defines}
  ${zxspectrum_hdmi_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumOlimexPcHdmiAudio720x576x50Hz PRIVATE
  ${olimex_pc_common_defines}
  ${olimex_pc_hdmi_audio_defines}
  ${zxspectrum_hdmi_720x576x50Hz_defines}
)

