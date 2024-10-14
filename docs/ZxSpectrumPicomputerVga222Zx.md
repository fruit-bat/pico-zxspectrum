### ZxSpectrumPicomputerZxLcd
This is a target written for Bobricius' PICOZX

<img src="picomputer_picozx.png" width="400"/>

It supports:
* USB keyboard
* Keyboard martix
* USB joysticks
* VGA video (RGB222)
* PWM sound (1 pin)
* SPI SD card

While on the menu...
* SHIFT-Fire = ESC
* RELOAD = Backspace
* SHIFT-RELOAD = Del

#### Circuit Diagram
![image](ZxSpectrumPicomputerVga222Zx.png)

The firmware produces RGB 222 for the VGA out:
![image](Pico%20VGA%20RGB222.png)

#### Firmware

| Display mode | Firmware |
| - | - |
| 720x576x50Hz | [ZxSpectrumPicomputerVga222Zx_720x576x50Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerVga222Zx_720x576x50Hz.uf2) |
| 640x480x60Hz | [ZxSpectrumPicomputerVga222Zx_640x480x60Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerVga222Zx_640x480x60Hz.uf2) |

#### References
[Hackaday](https://hackaday.io/project/186039-pico-zx-spectrum-128k)<br/>
[Adafruit](https://blog.adafruit.com/2022/07/27/picozx-is-a-128kb-zx-spectrum-clone-using-a-raspberry-pi-pico-retrocomputing-opensource-pico-dirkdierickx/)<br/>
[Hackster](https://www.hackster.io/news/pico-zx-spectrum-128k-is-a-recreation-of-the-sinclair-classic-computer-d51b59bca8d2)<br/>
[Reddit](https://www.reddit.com/r/zxspectrum/comments/w9bhwn/zx_spectrum_resurrections_by_raspberry_pico/)<br/>

