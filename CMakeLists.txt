# Replace TMDS with 10 bit UART (same baud rate):
# add_definitions(-DDVI_SERIAL_DEBUG=1)
# add_definitions(-DRUN_FROM_CRYSTAL)

add_executable(ZxSpectrum
	main.cpp
	Z80.cpp
	ZxSpectrumKeyboard.cpp
	ZxSpectrumHidKeyboard.cpp
	ZxSpectrum.cpp
	hid_app.c
	tmds_encode_zxspectrum.S
	tmds_encode_zxspectrum.h
#	hw_config.c
)

target_include_directories(ZxSpectrum PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(ZxSpectrum PRIVATE
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
	DVI_VERTICAL_REPEAT=2
	DVI_N_TMDS_BUFFERS=3
)

# add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

target_link_libraries(ZxSpectrum
	pico_stdlib
	pico_multicore
	pico_util
	libdvi
	tinyusb_host
	tinyusb_board
# FatFs_SPI
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrum)

