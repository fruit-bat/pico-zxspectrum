
### ZxSpectrum4PinAudioVga1111Ps2
This is a target similar to the HDMI prototype which uses VGA video output.<br/>
<img src="ZxSpectrum4PinAudioVga1111Ps2Breadboard.jpg" width="500"/>

It supports the following:
* USB keyboard
* PS/2 keyboard
* USB joysticks
* VGA video (RGBY1111)
* PWM sound (4 pin)
* SPI SD card
* Serial port debug

This target uses 4 audio pins:

#### Circuit diagrams
![image](ZxSpectrum4PinAudioVga1111Ps2.png)

The firmware produces RGBY 1111 for the VGA out:
![image](rgby_1111_vga.png)

#### Firmware

| Display mode | Firmware Pico | Firmware Pico 2 |
| - | - | - |
| 720x576x50Hz | [ZxSpectrum4PinAudioVga1111Ps2_720x576x50Hz.uf2](/uf2-rp2040/ZxSpectrum4PinAudioVga1111Ps2_720x576x50Hz.uf2) | [ZxSpectrum4PinAudioVga1111Ps2_720x576x50Hz.uf2](/uf2-rp2350-arm-s/ZxSpectrum4PinAudioVga1111Ps2_720x576x50Hz.uf2) |
| 640x480x60Hz | [ZxSpectrum4PinAudioVga1111Ps2_640x480x60Hz.uf2](/uf2-rp2040/ZxSpectrum4PinAudioVga1111Ps2_640x480x60Hz.uf2) | [ZxSpectrum4PinAudioVga1111Ps2_640x480x60Hz.uf2](/uf2-rp2350-arm-s/ZxSpectrum4PinAudioVga1111Ps2_640x480x60Hz.uf2) |