### ZxSpectrumPicomputerZxLcd
This is a target written for Bobricius' PICOZX with built in LCD 

<img src="picozxlcd.png" width="400"/>

It supports:
* USB keyboard
* Keyboard martix
* USB joysticks
* VGA video (RGB222)
* LCD video (ST7789/ILI9341)
* PWM sound (1 pin)
* SPI SD card

By default, the board starts up using the LCD as its display.
To boot into VGA mode hold down the 'fire' button during reset.
Placing vga.txt on the SD card in the zxspectrum folder changes the default to VGA.


While on the menu...
* SHIFT-Fire = ESC
* RELOAD = Backspace
* SHIFT-RELOAD = Del

#### Circuit Diagrams
![image](ZxSpectrumPicomputerZxLcd.png)

![image](ZxSpectrumPicomputerZxLcdSchematic.png)

The firmware produces RGB 222 for the VGA out:<br/>
![image](Pico%20VGA%20RGB222.png)

#### Firmware
| LCD | Colour Encoding | Colour Channel | VGA | Board | Firmware |
| - | - | - | - | - | - |
| ST7789 | BGR | Normal | 640x480x60Hz |Bobricius | [ZxSpectrumPicomputerZxLcd_640x480x60Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerZxLcd_640x480x60Hz.uf2) |
| ST7789 | BGR | Normal | 720x576x50Hz |Bobricius | [ZxSpectrumPicomputerZxLcd_720x576x50Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerZxLcd_720x576x50Hz.uf2) |
| ST7789 | BGR | Inverted | 640x480x60Hz | | [ZxSpectrumPicomputerZxInverseLcd_640x480x60Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerZxInverseLcd_640x480x60Hz.uf2) |
| ST7789 | BGR | Inverted | 720x576x50Hz | | [ZxSpectrumPicomputerZxInverseLcd_720x576x50Hz.uf2](/uf2-rp2040/xSpectrumPicomputerZxInverseLcd_720x576x50Hz.uf2) |
| ILI9341 | RGB | inverted | 640x480x60Hz | | [ZxSpectrumPicomputerZxILI9341Lcd_640x480x60Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerZxILI9341Lcd_640x480x60Hz.uf2) |
| ILI9341 | RGB | inverted | 720x576x50Hz | | [ZxSpectrumPicomputerZxILI9341Lcd_720x576x50Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerZxILI9341Lcd_720x576x50Hz.uf2) |

#### Build system notes
The make system has some switches to adapt it to different LCD panels:
| Symbol | Description | PICOZX | ILI19341 |
| ------ | ----------- | ------ | -------- |
| LCD_INVERSE | invert the brightness of the RGB color components | undefined | defined |
| LCD_MIRROR_X | Mirror the LCD output horizontally | defined | undefined |
| LCD_RGB | Use RBG order rather than BGR | undefined |  defined |

#### References
[ILI9341 datasheet](https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf)<br/>
[hackster](https://www.hackster.io/news/peter-bobricius-misenko-s-picozx-lcd-is-the-handheld-sinclair-zx-spectrum-the-1980s-never-got-24c00f395b9d)<br/>
[raspberrypi.com](https://www.raspberrypi.com/news/build-your-own-handheld-zx-spectrum-with-raspberry-pi-pico/)<br/>
[Bobricius on YouTube](https://www.youtube.com/watch?v=AbfBHwBqbpY)<br/>
[Bobricius PCBWay](https://www.pcbway.com/project/shareproject/PICO_ZX_LCD_VGA_Spectrum_128k_094be579.html)<br/>
[Pixel Fandango on YouTube](https://www.youtube.com/watch?v=y6s7uneGPm8)<br/>

