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

get_filename_component(PICO_FOLDER ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)

include_directories(
	${PICO_FOLDER}/PicoDVI/software/include
)


add_subdirectory(${PICO_FOLDER}/PicoDVI/software/libdvi "${CMAKE_CURRENT_BINARY_DIR}/libdvi")
add_subdirectory(${PICO_FOLDER}/PicoDVI/software/libsprite "${CMAKE_CURRENT_BINARY_DIR}/libsprite")
add_subdirectory(${PICO_FOLDER}/pico-vga-332/ "${CMAKE_CURRENT_BINARY_DIR}/pico-vga-332")
add_subdirectory(${PICO_FOLDER}/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI "${CMAKE_CURRENT_BINARY_DIR}/FatFs_SPI")
add_subdirectory(${PICO_FOLDER}/pico-dvi-menu "${CMAKE_CURRENT_BINARY_DIR}/pico-dvi-menu")
add_subdirectory(${PICO_FOLDER}/pico-emu-utils "${CMAKE_CURRENT_BINARY_DIR}/pico-emu-utils")

add_subdirectory(src  "${CMAKE_CURRENT_BINARY_DIR}/bin")

