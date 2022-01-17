# Replace TMDS with 10 bit UART (same baud rate):
# add_definitions(-DDVI_SERIAL_DEBUG=1)
# add_definitions(-DRUN_FROM_CRYSTAL)

add_executable(ZxSpectrum
	src/main.cpp
	src/Z80.cpp
	src/ZxSpectrumKeyboard.cpp
	src/ZxSpectrumHidKeyboard.cpp
	src/ZxSpectrum.cpp
	src/hid_app.c
	src/tmds_encode_zxspectrum.S
	src/tmds_encode_zxspectrum.h
	src/InputStream.cpp
	src/OutputStream.cpp
	src/SizingOutputStream.cpp
	src/ArrayInputStream.cpp
	src/BufferedInputStream.cpp
	src/hw_config.c
	src/SdCardFatFsSpi.cpp
	src/FatFsSpiInputStream.cpp
	src/FatFsSpiOutputStream.cpp
	src/QuickSave.cpp
	src/Pulse.cpp
	src/PulseByte.cpp
	src/PulseByteStream.cpp
	src/PulsePreamble.cpp
	src/PulseBlock.cpp
	src/ZxSpectrumFile.cpp
	src/ZxSpectrumFileLoop.cpp
	src/ZxSpectrumFatFsSpiFileLoop.cpp
	src/ZxSpectrumAy.cpp
	src/ZxSpectrumMenu.cpp
)

target_include_directories(ZxSpectrum PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

target_compile_definitions(ZxSpectrum PRIVATE
	DVI_DEFAULT_SERIAL_CONFIG=${DVI_DEFAULT_SERIAL_CONFIG}
	DVI_VERTICAL_REPEAT=2
	DVI_N_TMDS_BUFFERS=3
	DVI_1BPP_BIT_REVERSE=1
)

target_link_libraries(ZxSpectrum
	pico_stdlib
	pico_multicore
	pico_util
	libdvi
	tinyusb_host
	tinyusb_board
	FatFs_SPI
	pico_dvi_menu
)

# create map/bin/hex file etc.
pico_add_extra_outputs(ZxSpectrum)

