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
	InputStream.cpp
	OutputStream.cpp
	ArrayInputStream.cpp
	BufferedInputStream.cpp
	hw_config.c
	SdCardFatFsSpi.cpp
	FatFsSpiInputStream.cpp
	FatFsSpiOutputStream.cpp
	QuickSave.cpp
	Pulse.cpp
	PulseByte.cpp
	PulseByteStream.cpp
	PulsePreamble.cpp
	PulseBlock.cpp
	ZxSpectrumFile.cpp
	ZxSpectrumFileLoop.cpp
	ZxSpectrumFatFsSpiFileLoop.cpp
)

target_include_directories(ZxSpectrum PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(ZxSpectrum PRIVATE
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
	DVI_VERTICAL_REPEAT=2
	DVI_N_TMDS_BUFFERS=3
)

# Oh dear, it looks like FatFs_SPI should be built in the parent
# currently this is built as part of sorcerer2... clearly it belongs somewhere else!
# for now, if not also building sorcerer2 uncomment the line below
# add_subdirectory(no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

target_link_libraries(ZxSpectrum
	pico_stdlib
	pico_multicore
	pico_util
	libdvi
	tinyusb_host
	tinyusb_board
  FatFs_SPI
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrum)

