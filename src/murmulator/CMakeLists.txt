include_directories(${CMAKE_CURRENT_LIST_DIR})

#set(picomputer_common_src
#	${CMAKE_CURRENT_LIST_DIR}/ps2kbd.cpp
#	${CMAKE_CURRENT_LIST_DIR}/ps2kbd.h
#)

add_subdirectory(murmulator_hdmi)
add_subdirectory(murmulator_vga)
