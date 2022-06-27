include_directories(${CMAKE_CURRENT_LIST_DIR})

set(picomputer_common_src
	${CMAKE_CURRENT_LIST_DIR}/pzx_keyscan.cpp
	${CMAKE_CURRENT_LIST_DIR}/pzx_keyscan.h
	${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPicomputerJoystick.cpp
	${CMAKE_CURRENT_LIST_DIR}/hw_config.c
)

add_subdirectory(picomputer_vga)
add_subdirectory(picomputer_vga_zx)
add_subdirectory(picomputer_st7789)
