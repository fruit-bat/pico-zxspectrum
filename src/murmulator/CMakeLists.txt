include_directories(${CMAKE_CURRENT_LIST_DIR})

set(murmulator_common_src
	${CMAKE_CURRENT_LIST_DIR}/ps2kbd_mrmltr.cpp
	${CMAKE_CURRENT_LIST_DIR}/ps2kbd_mrmltr.h
)

set(murmulator_ps2kbd_pio
  ${CMAKE_CURRENT_LIST_DIR}/ps2kbd_mrmltr.pio
)


add_subdirectory(murmulator_hdmi)
add_subdirectory(murmulator_vga)
