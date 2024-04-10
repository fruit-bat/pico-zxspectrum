### ZxSpectrumBreadboardHdmiNPinAudio
This is a series of targets based around my original breadboard prototype:

<img src="pico_zxspectrum_prototype_1.jpg" height="200"/>

Support for the following:
* USB keyboard
* PS/2 keyboard
* USB joysticks
* 50Hz/60Hz HDMI video
* HDMI/PWM audio
* Audio input (load from tape)
* SPI SD card
* Serial port debug
* *Special* support for Ringo low res mode

All of these targets share the same pinout but make different use of the 4 audio pins:

#### Circuit diagrams

![image](ZxSpectrumBreadboardHdmi4PinAudio.png)

#### Firmware
| Audio | Display mode | Firmware |
| - | - | - |
| HDMI | 720x540x50Hz | [ZxSpectrumBreadboardHdmiAudio720x540x50Hz.uf2](/uf2/ZxSpectrumBreadboardHdmiAudio720x540x50Hz.uf2) |
| HDMI | 640x480x60Hz | [ZxSpectrumBreadboardHdmiAudio.uf2](/uf2/ZxSpectrumBreadboardHdmiAudio.uf2) |
| 1 pin PWM | 640x480x60Hz | [ZxSpectrumBreadboardHdmi1PinAudio.uf2](/uf2/ZxSpectrumBreadboardHdmi1PinAudio.uf2) |
| 2 pin PWM | 640x480x60Hz | [ZxSpectrumBreadboardHdmi2PinAudio.uf2](/uf2/ZxSpectrumBreadboardHdmi2PinAudio.uf2) |
| 4 pin PWM | 640x480x60Hz | [ZxSpectrumBreadboardHdmi4PinAudio.uf2](/uf2/ZxSpectrumBreadboardHdmi4PinAudio.uf2) |
