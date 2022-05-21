
add_executable(ZxSpectrumPicomputerVga
  ${zxspectrum_common_src}
	main.cpp
	hw_config.c
)

# TODO environment variable and relative paths
add_subdirectory(/home/pi/pico/pico-vga-332/ "${CMAKE_CURRENT_BINARY_DIR}/pico-vga-332")

target_compile_definitions(ZxSpectrumPicomputerVga PRIVATE
  -DPCS_COLS=80 -DPCS_ROWS=30
)

target_link_libraries(ZxSpectrumPicomputerVga
  ${zxspectrum_common_libs}
	pico_emu_utils
  pico_vga_menu
  pico_vga_332
	tinyusb_host
	tinyusb_board
  hardware_pio
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrumPicomputerVga)

