foreach(target
  ZxSpectrumPicomputerVga_640x480x60Hz
  ZxSpectrumPicomputerVga_720x576x50Hz
)
  add_executable(${target}
    ${zxspectrum_common_src}
    ${zxspectrum_rgb_src}
    ${picomputer_common_src}
    ${zxspectrum_scanvideo_src}
    main.cpp
  )

  target_link_libraries(${target}
    ${zxspectrum_common_libs}
    pico_emu_utils
    pico_scanvideo_menu
    pico_scanvideo_dpi
    tinyusb_host
    tinyusb_board
    hardware_pio
    hardware_pwm
  )

  pico_enable_stdio_usb(${target} 0)
  pico_enable_stdio_uart(${target} 0)

  # create map/bin/hex file etc.
  pico_add_extra_outputs(${target})

endforeach()

set(ZxSpectrumPicomputerVga_defines
  ${picomputer_common_defines}
  SPK_PIN=9
  VGA_ENC_RGB_332
  PICO_SCANVIDEO_COLOR_PIN_COUNT=8u
  PICO_SCANVIDEO_DPI_PIXEL_RCOUNT=3u
  PICO_SCANVIDEO_DPI_PIXEL_GCOUNT=3u
  PICO_SCANVIDEO_DPI_PIXEL_BCOUNT=2u
  PICO_SCANVIDEO_DPI_PIXEL_RSHIFT=5u
  PICO_SCANVIDEO_DPI_PIXEL_GSHIFT=2u
  PICO_SCANVIDEO_DPI_PIXEL_BSHIFT=0u
  PICO_SCANVIDEO_COLOR_PIN_BASE=0u
  PICOMPUTER_VGA
)

set(ZxSpectrumPicomputerVga_640x480x60Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=80
  PCS_ROWS=30
  # Set the VGA timing
  VGA_MODE=vga_mode_640x240_60
  # The display resolution
  DISPLAY_WIDTH_PIXELS=640
  DISPLAY_HEIGHT_PIXELS=480
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToCpu
  # TODO work what this is
  PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=700
)

set(ZxSpectrumPicomputerVga_720x576x50Hz_defines
  # Set the resolution of the menu screen
  PCS_COLS=90
  PCS_ROWS=30
  SZ_FRAME_X=5
  # Set the VGA timing
  VGA_MODE=vga_mode_720x288_50
  # The display resolution
  DISPLAY_WIDTH_PIXELS=720
  DISPLAY_HEIGHT_PIXELS=576
  # The default interrupt source
  ZX_SPECTRUM_INT_SRC=SyncToDisplay
  # TODO work what this is
  PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=800
)

target_compile_definitions(ZxSpectrumPicomputerVga_640x480x60Hz PRIVATE
  ${ZxSpectrumPicomputerVga_defines}
  ${ZxSpectrumPicomputerVga_640x480x60Hz_defines}
)

target_compile_definitions(ZxSpectrumPicomputerVga_720x576x50Hz PRIVATE
  ${ZxSpectrumPicomputerVga_defines}
  ${ZxSpectrumPicomputerVga_720x576x50Hz_defines}
)

