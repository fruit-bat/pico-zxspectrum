include_directories(${CMAKE_CURRENT_LIST_DIR})

set(picomputer_common_defines
  # Tell FATFS we want to find files
  FF_USE_FIND=1  
  # Configure the SPI sd card to use PIO1
  SDCARD_PIO=pio1
  SDCARD_PIO_SM=1  
  SDCARD_PIN_SPI0_CS=13
  SDCARD_PIN_SPI0_SCK=10
  SDCARD_PIN_SPI0_MOSI=11
  SDCARD_PIN_SPI0_MISO=12
)

set(picomputer_common_src
	${CMAKE_CURRENT_LIST_DIR}/pzx_keyscan.cpp
	${CMAKE_CURRENT_LIST_DIR}/pzx_keyscan.h
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPicomputerJoystick.cpp
)

add_subdirectory(picomputer_vga)
add_subdirectory(picomputer_vga_zx)
add_subdirectory(picomputer_st7789)
