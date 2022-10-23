
include_directories(${CMAKE_CURRENT_LIST_DIR})

set(zxspectrum_common_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAudio.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKiosk.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFatSpiKiosk.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Z80.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKeyboard.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumHidKeyboard.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumHidJoystick.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumDualJoystick.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrum.cpp
  ${CMAKE_CURRENT_LIST_DIR}/QuickSave.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProc.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTone.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcStdHeader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcStdByte.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcStdByteStream.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcChain.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTap.cpp

  ${CMAKE_CURRENT_LIST_DIR}/Pulse.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseByte.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseByteStream.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulsePreamble.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseBlock.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseCpu.cpp  
  ${CMAKE_CURRENT_LIST_DIR}/PulseTzx.cpp  
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFile.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFileLoop.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFatFsSpiFileLoop.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAy.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumMenu.cpp
)

set(zxspectrum_rgb_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPrepareRgbScanline.cpp
)

set(zxspectrum_scanvideo_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPrepareScanvideoScanline.cpp
)

set(zxspectrum_dvi_src
  ${CMAKE_CURRENT_LIST_DIR}/tmds_encode_zxspectrum.S
  ${CMAKE_CURRENT_LIST_DIR}/tmds_encode_zxspectrum.h
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPrepareDviScanline.cpp
)

set(zxspectrum_kbd_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKeyMatrix.cpp
)

set(zxspectrum_common_libs
  pico_stdlib
  pico_multicore
  pico_util
  pico_emu_utils
  sdcard
  fatfs
  hardware_dma
)

set(zxspectrum_audio_i2s_pio
  ${CMAKE_CURRENT_LIST_DIR}/audio_i2s.pio
)

set(zxspectrum_ear_in_pio
  ${CMAKE_CURRENT_LIST_DIR}/zx_ear_in.pio
)

add_subdirectory(hdmi)
add_subdirectory(picomputer)
add_subdirectory(vga)
add_subdirectory(picovga)

