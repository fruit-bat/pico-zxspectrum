
include_directories(${CMAKE_CURRENT_LIST_DIR})

# TODO environment variable and relative paths
# add_subdirectory(/home/pi/pico/PicoDVI/software/apps/pico-dvi-menu/ "${CMAKE_CURRENT_BINARY_DIR}/pico-dvi-menu")

set(zxspectrum_common_src
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKiosk.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFatSpiKiosk.cpp
	${CMAKE_CURRENT_LIST_DIR}/Z80.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKeyboard.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumHidKeyboard.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumHidJoystick.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumDualJoystick.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrum.cpp
	${CMAKE_CURRENT_LIST_DIR}/QuickSave.cpp
	${CMAKE_CURRENT_LIST_DIR}/Pulse.cpp
	${CMAKE_CURRENT_LIST_DIR}/PulseByte.cpp
	${CMAKE_CURRENT_LIST_DIR}/PulseByteStream.cpp
	${CMAKE_CURRENT_LIST_DIR}/PulsePreamble.cpp
	${CMAKE_CURRENT_LIST_DIR}/PulseBlock.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFile.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFileLoop.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFatFsSpiFileLoop.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAy.cpp
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumMenu.cpp
)

set(zxspectrum_common_libs
	pico_stdlib
	pico_multicore
	pico_util
	FatFs_SPI
	pico_emu_utils
)

set(zxspectrum_common_include
  ${CMAKE_CURRENT_LIST_DIR}/src
)

add_subdirectory(breadboard_hdmi)
add_subdirectory(picomputer_vga)
