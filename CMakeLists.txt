cmake_minimum_required(VERSION 3.12)

# Determine the MCU type and configure accordingly
# set(PICO_MCU "rp2040" CACHE STRING "Specify Pico MCU type")

# if ((${PICO_MCU} STREQUAL "rp2350"))
#     set(PICO_PLATFORM "rp2350-arm-s")
#     add_compile_definitions(PICO_MCU="RP2350")
# else()
#     set(PICO_PLATFORM "rp2040")
#     add_compile_definitions(PICO_MCU="RP2040")
# endif()

# Look for board file in local board directory
set(PICO_BOARD_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/boards)
if (NOT PICO_BOARD)
    set(PICO_BOARD "pico")
endif()

# Pico SDC on Raspberry Pi 4
cmake_policy(VERSION 3.12)

# Pico SDC Toolchai on Windows 10
# cmake_policy(SET CMP0115 OLD)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
include(pico_extras_import.cmake)

project(picodvi C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Current version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

add_compile_definitions(PICO_MCU="${PICO_PLATFORM}")

add_compile_options(-Wall)
add_definitions(-DHID_PROTOCOL_DEFAULT=HID_PROTOCOL_REPORT)

set(DVI_DEFAULT_SERIAL_CONFIG "pico_sock_cfg" CACHE STRING
	"Select a default pin configuration from common_dvi_pin_configs.h")

get_filename_component(PICO_FOLDER ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)

include_directories(
	${PICO_FOLDER}/PicoDVI/software/include
)

add_subdirectory(${PICO_FOLDER}/PicoDVI/software/libdvi "${CMAKE_CURRENT_BINARY_DIR}/libdvi")
add_subdirectory(${PICO_FOLDER}/PicoDVI/software/libsprite "${CMAKE_CURRENT_BINARY_DIR}/libsprite")
add_subdirectory(${PICO_FOLDER}/pico-dvi-menu "${CMAKE_CURRENT_BINARY_DIR}/pico-dvi-menu")
add_subdirectory(${PICO_FOLDER}/pico-emu-utils "${CMAKE_CURRENT_BINARY_DIR}/pico-emu-utils")
add_subdirectory(${PICO_FOLDER}/pico-hid-host "${CMAKE_CURRENT_BINARY_DIR}/pico-hid-host")
add_subdirectory(${PICO_FOLDER}/pico-nespad "${CMAKE_CURRENT_BINARY_DIR}/pico-nespad")

add_subdirectory(${PICO_FOLDER}/pimoroni-pico/drivers/fatfs "${CMAKE_CURRENT_BINARY_DIR}/pimoroni-pico/drivers/fatfs")
add_subdirectory(${PICO_FOLDER}/pimoroni-pico/drivers/sdcard "${CMAKE_CURRENT_BINARY_DIR}/pimoroni-pico/drivers/sdcard")
include_directories(${PICO_FOLDER}/pimoroni-pico/drivers/fatfs)
include_directories(${PICO_FOLDER}/pimoroni-pico/drivers/sdcard)

add_subdirectory(src  "${CMAKE_CURRENT_BINARY_DIR}/bin")

add_subdirectory(${PICO_FOLDER}/Zeta "${CMAKE_CURRENT_BINARY_DIR}/Zeta")

set(Z80_SHARED_LIBS                 NO  CACHE BOOL "")
set(Z80_WITH_Q                      YES CACHE BOOL "")
set(Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG YES CACHE BOOL "")
set(Zeta_INCLUDE_DIR                "${PICO_FOLDER}/Zeta")
set(Zeta_VERSION                    "v0.1")

add_subdirectory(${PICO_FOLDER}/Z80 "${CMAKE_CURRENT_BINARY_DIR}/Z80")
include_directories(${PICO_FOLDER}/Z80/API)
