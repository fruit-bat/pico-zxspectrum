cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)

project(picodvi C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_compile_options(-Wall)


set(DVI_DEFAULT_SERIAL_CONFIG "pico_sock_cfg" CACHE STRING
	"Select a default pin configuration from common_dvi_pin_configs.h")

include_directories(
	/home/pi/pico/PicoDVI/software/include
)


add_subdirectory(/home/pi/pico/PicoDVI/software/libdvi "${CMAKE_CURRENT_BINARY_DIR}/libdvi")
add_subdirectory(/home/pi/pico/PicoDVI/software/libsprite "${CMAKE_CURRENT_BINARY_DIR}/libsprite")
add_subdirectory(/home/pi/pico/pico-vga-332/ "${CMAKE_CURRENT_BINARY_DIR}/pico-vga-332")
add_subdirectory(/home/pi/pico/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI "${CMAKE_CURRENT_BINARY_DIR}/FatFs_SPI")
add_subdirectory(/home/pi/pico/pico-dvi-menu "${CMAKE_CURRENT_BINARY_DIR}/pico-dvi-menu")
add_subdirectory(/home/pi/pico/pico-emu-utils "${CMAKE_CURRENT_BINARY_DIR}/pico-emu-utils")

add_subdirectory(src  "${CMAKE_CURRENT_BINARY_DIR}/bin")

