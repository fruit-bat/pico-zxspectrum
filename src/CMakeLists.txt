
include_directories(${CMAKE_CURRENT_LIST_DIR})

set(zxspectrum_st7789_lcd_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSt7789LcdRenderLoop.cpp
  ${CMAKE_CURRENT_LIST_DIR}/picomputer/picomputer_st7789/pzx_prepare_rgb444_scanline.cpp # TODO Move this file
  ${CMAKE_CURRENT_LIST_DIR}/picomputer/picomputer_st7789/st7789_lcd.pio # TODO Move this file
  ${CMAKE_CURRENT_LIST_DIR}/picomputer/picomputer_st7789/st7789_lcd.cpp # TODO Move this file
)

set(zxspectrum_common_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumMouse.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumHidMouse.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumSettings.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFlashSettings.cpp # Needed for flash settings (not currently working)
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFileSettings.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAudio.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAudioVol.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAudioPioPwm.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAudioPwm.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAudioI2s.cpp
  #${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAudioHdmi.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKiosk.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFatSpiKiosk.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKeyboard.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumHidKeyboard.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumJoystick.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumHidJoystick.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumDualJoystick.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumNespadJoystick.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrum.cpp
  ${CMAKE_CURRENT_LIST_DIR}/QuickSave.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProc.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTone.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcStdHeader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcStdByte.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcStdByteStream.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcChain.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxBlock.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxTurbo.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTap.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzx.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxHeader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxGlue.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxIndex.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcPauseMillis.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxPureTone.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcPulseStream.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxPulseSequence.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxPureData.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcBitStream.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxDirectRecording.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcCallStream.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxGenData.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcTzxSelect.cpp 
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcSymdefs.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcRleSym.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcSymbol.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PulseProcSymbolData.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFile.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFileLoop.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumAy.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumMenu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumFatSpiExists.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PicoOnScreenKeyboard.cpp
)

set(zxspectrum_rgb332_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxRgb332RenderLoop.cpp
)

set(zxspectrum_rgb_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPrepareRgbScanline.cpp
)

set(zxspectrum_scanvideo_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPrepareScanvideoScanline.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxScanlineVgaRenderLoop.cpp
)

set(zxspectrum_dvi_src
  ${CMAKE_CURRENT_LIST_DIR}/tmds_encode_zxspectrum.S
  ${CMAKE_CURRENT_LIST_DIR}/tmds_encode_zxspectrum.h
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumPrepareDviScanline.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxDviRenderLoop.cpp
)

set(zxspectrum_kbd_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKeyMatrix.cpp
)

set(zxspectrum_kbd2_src
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumKeyMatrix2.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ZxSpectrumMatrixJoystick.cpp
)

set(zxspectrum_common_libs
  pico_stdlib
  pico_multicore
  pico_util
  pico_emu_utils
  pico_hid_host
  pico_nespad
  sdcard
  fatfs
  hardware_dma
  hardware_flash # Needed for flash settings (not currently working)
  hardware_sync  # Needed for flash settings (not currently working)
  hardware_pio
  hardware_pwm
  hardware_adc  
  Zeta
  Z80
)

set(zxspectrum_audio_i2s_pio
  ${CMAKE_CURRENT_LIST_DIR}/audio_i2s.pio
)

set(zxspectrum_ear_in_pio
  ${CMAKE_CURRENT_LIST_DIR}/zx_ear_in.pio
)

set(zxspectrum_pwm_pio
  ${CMAKE_CURRENT_LIST_DIR}/pwm.pio
)

set(zxspectrum_st7789_pio 
  ${CMAKE_CURRENT_LIST_DIR}/picomputer/picomputer_st7789/st7789_lcd.pio # TODO Move this file
)

set(zxspectrum_pio_all
  ${zxspectrum_audio_i2s_pio}
  ${zxspectrum_ear_in_pio}
  ${zxspectrum_pwm_pio}
  ${zxspectrum_st7789_pio}
)

add_subdirectory(hdmi)
add_subdirectory(picomputer)
add_subdirectory(vga)
add_subdirectory(picovga)

add_custom_target (relevant)

########################################################################
# MURMULATOR (RP2040 or RP2350)
########################################################################
add_custom_target(ZX-MURMULATOR)
add_dependencies(ZX-MURMULATOR
  ZX-MURMULATOR_HDMI_PWM_AUDIO_640x480x60Hz
  ZX-MURMULATOR_HDMI_PWM_AUDIO_720x576x50Hz
  ZX-MURMULATOR_HDMI_HDMI_AUDIO_640x480x60Hz
  ZX-MURMULATOR_HDMI_HDMI_AUDIO_720x576x50Hz
  ZX-MURMULATOR_HDMI_I2S_AUDIO_640x480x60Hz
  ZX-MURMULATOR_HDMI_I2S_AUDIO_720x576x50Hz
  ZX-MURMULATOR_VGA_PWM_AUDIO_640x480x60Hz
  ZX-MURMULATOR_VGA_PWM_AUDIO_720x576x50Hz
  ZX-MURMULATOR_VGA_I2S_AUDIO_640x480x60Hz
  ZX-MURMULATOR_VGA_I2S_AUDIO_720x576x50Hz
)
if ((${PICO_BOARD} STREQUAL "pico") OR (${PICO_BOARD} STREQUAL "pico2"))
  message("Relevant target: ZX-MURMULATOR")
  add_dependencies(relevant ZX-MURMULATOR)
endif()

########################################################################
# MURMULATOR2 (RP2040 or RP2350)
########################################################################
add_custom_target(ZX-MURMULATOR2)
add_dependencies(ZX-MURMULATOR2
  ZX-MURMULATOR2_HDMI_PWM_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_HDMI_PWM_AUDIO_720x576x50Hz
  ZX-MURMULATOR2_HDMI_HDMI_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_HDMI_HDMI_AUDIO_720x576x50Hz
  ZX-MURMULATOR2_HDMI_I2S_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_HDMI_I2S_AUDIO_720x576x50Hz
  ZX-MURMULATOR2_VGA_PWM_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_VGA_PWM_AUDIO_720x576x50Hz
  ZX-MURMULATOR2_VGA_I2S_AUDIO_640x480x60Hz
  ZX-MURMULATOR2_VGA_I2S_AUDIO_720x576x50Hz
)
if ((${PICO_BOARD} STREQUAL "pico") OR (${PICO_BOARD} STREQUAL "pico2"))
  message("Relevant target: ZX-MURMULATOR2")
  add_dependencies(relevant ZX-MURMULATOR2)
endif()

########################################################################
# Breadboard (RP2040 or RP2350)
########################################################################
add_custom_target (ZxSpectrumBreadboard)
add_dependencies (ZxSpectrumBreadboard
  ZxSpectrumBreadboardHdmiAudio
  ZxSpectrumBreadboardHdmiAudio720x540x50Hz
  ZxSpectrumBreadboardHdmiAudio720x576x50Hz
  ZxSpectrumBreadboardHdmi1PinAudio
  ZxSpectrumBreadboardHdmi2PinAudio
  ZxSpectrumBreadboardHdmi4PinAudio
  ZxSpectrumBreadboardHdmiKbd1PinAudio
  ZxSpectrum4PinAudioVga1111Ps2_640x480x60Hz
  ZxSpectrum4PinAudioVga1111Ps2_720x576x50Hz
  ZxSpectrum4PinAudioVga222Ps2_640x480x60Hz
  ZxSpectrum4PinAudioVga222Ps2_720x576x50Hz
  ZxSpectrumI2SAudioVga1111Ps2_CVBSx50Hz
)
if ((${PICO_BOARD} STREQUAL "pico") OR (${PICO_BOARD} STREQUAL "pico2"))
  message("Relevant target: ZxSpectrumBreadboard")
  add_dependencies(relevant ZxSpectrumBreadboard)
endif()

########################################################################
# Pimoroni pico dv (RP2040 or RP2350)
########################################################################
add_custom_target (ZxSpectrumPicoDv)
add_dependencies (ZxSpectrumPicoDv
  ZxSpectrumPicoDv_I2SAudio_640x480x60Hz
  ZxSpectrumPicoDv_HdmiAudio_640x480x60Hz
  ZxSpectrumPicoDv_I2SAudio_720x576x50Hz
  ZxSpectrumPicoDv_HdmiAudio_720x576x50Hz
)
if ((${PICO_BOARD} STREQUAL "pico") OR (${PICO_BOARD} STREQUAL "pico2"))
  message("Relevant target: ZxSpectrumPicoDv")
  add_dependencies(relevant ZxSpectrumPicoDv)
endif()

########################################################################
# Adafruit feather (RP2040 only)
# cmake -DPICO_COPY_TO_RAM=0 -DPICO_BOARD=adafruit_feather_rp2040 ..
########################################################################
add_custom_target (ZxSpectrumAdafruitFeather)
add_dependencies (ZxSpectrumAdafruitFeather
  ZxSpectrumAdafruitFeatherHdmi
)
if ((${PICO_PLATFORM} STREQUAL "rp2040") AND (${PICO_BOARD} STREQUAL "adafruit_feather_rp2040"))
  message("Relevant target: ZxSpectrumAdafruitFeather")
  add_dependencies(relevant ZxSpectrumAdafruitFeather)
endif()

########################################################################
# Waveshare - RP2040 - PicoZero (RP2040 only)
########################################################################
add_custom_target (ZxSpectrumPiZero)
add_dependencies (ZxSpectrumPiZero
  ZxSpectrumPiZero_640x480x60Hz
  ZxSpectrumPiZero_720x576x50Hz
)
if (${PICO_BOARD} STREQUAL "pico")
  message("Relevant target: ZxSpectrumPiZero")
  add_dependencies(relevant ZxSpectrumPiZero)
endif()

########################################################################
# Olimex RP2040-PICO-PC (RP2040 or RP2350)
########################################################################
add_custom_target (ZxSpectrumOlimexPc)
add_dependencies (ZxSpectrumOlimexPc
  ZxSpectrumOlimexPcHdmiAudio640x480x60Hz
  ZxSpectrumOlimexPcHdmiAudio720x576x50Hz
)
if ((${PICO_BOARD} STREQUAL "pico") OR (${PICO_BOARD} STREQUAL "pico2"))
  message("Relevant target: ZxSpectrumOlimexPc")
  add_dependencies(relevant ZxSpectrumOlimexPc)
endif()

########################################################################
# Pimoroni Pico VGA Demo Base (RP2040 or RP2350)
# cmake -DPICO_COPY_TO_RAM=0 -DPICO_PLATFORM=rp2040 -DPICO_BOARD=vgaboard ../..
# cmake -DPICO_COPY_TO_RAM=0 -DPICO_PLATFORM=rp2350 -DPICO_BOARD=vgaboard ../..
########################################################################
add_custom_target (ZxSpectrumPicoVga)
add_dependencies (ZxSpectrumPicoVga
  ZxSpectrumPicoVga_640x480x60Hz
  ZxSpectrumPicoVga_720x576x50Hz
)
if (${PICO_BOARD} STREQUAL "vgaboard")
  message("Relevant target: ZxSpectrumPicoVga")
  add_dependencies(relevant ZxSpectrumPicoVga)
endif()

########################################################################
# Pico Mite VGA (RP2040 or RP2350)
########################################################################
add_custom_target (ZxSpectrumPicoMite)
add_dependencies (ZxSpectrumPicoMite
  ZxSpectrumPicoMiteVGA_640x480x60Hz
  ZxSpectrumPicoMiteVGA_720x576x50Hz
)
if ((${PICO_BOARD} STREQUAL "pico") OR (${PICO_BOARD} STREQUAL "pico2"))
  message("Relevant target: ZxSpectrumPicoMite")
  add_dependencies(relevant ZxSpectrumPicoMite)
endif()

########################################################################
# Picomputer (RP2040 Only)
########################################################################
add_custom_target (ZxSpectrumPicomputer)
add_dependencies (ZxSpectrumPicomputer
  ZxSpectrumPicomputerZxLcd_640x480x60Hz
  ZxSpectrumPicomputerZxLcd_720x576x50Hz
  ZxSpectrumPicomputerZxInverseLcd_640x480x60Hz
  ZxSpectrumPicomputerZxInverseLcd_720x576x50Hz
  ZxSpectrumPicomputerZxILI9341Lcd_720x576x50Hz
  ZxSpectrumPicomputerZxILI9341Lcd_640x480x60Hz
  ZxSpectrumPicomputerVga_640x480x60Hz
  ZxSpectrumPicomputerVga_720x576x50Hz
  ZxSpectrumPicomputerVga1111Zx_640x480x60Hz
  ZxSpectrumPicomputerVga222Zx_640x480x60Hz
  ZxSpectrumPicomputerVgaAuk_640x480x60Hz
  ZxSpectrumPicomputerVgaAukBob_640x480x60Hz
  ZxSpectrumPicomputerVga1111Zx_720x576x50Hz
  ZxSpectrumPicomputerVga222Zx_720x576x50Hz
  ZxSpectrumPicomputerVgaAuk_720x576x50Hz
  ZxSpectrumPicomputerVgaAukBob_720x576x50Hz
  ZxSpectrumPicomputerMax
  ZxSpectrumPicomputerZX
  # this one is not like the others... made for emilen72 
  ZxSpectrumPicomputerVga1111AukBob_720x576x50Hz  
)
if (${PICO_BOARD} STREQUAL "pico")
  message("Relevant target: ZxSpectrumPicomputer")
  add_dependencies(relevant ZxSpectrumPicomputer)
endif()
########################################################################
# Picomputer 2 II (RP2350 Only)
########################################################################
add_custom_target (ZxSpectrumPicomputer2)
add_dependencies (ZxSpectrumPicomputer2
  ZxSpectrumPicomputerZx2Lcd_640x480x60Hz
  ZxSpectrumPicomputerZx2Lcd_720x576x50Hz
)
if (${PICO_BOARD} STREQUAL "pico2")
  message("Relevant target: ZxSpectrumPicomputer2")
  add_dependencies(relevant ZxSpectrumPicomputer2)
endif()


