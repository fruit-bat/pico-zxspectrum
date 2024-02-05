### ZxSpectrumPicomputerZxLcd
This is a target written for Bobricius' PICOZX with built in LCD 

<img src="picozxlcd.png" width="300"/>

It supports:
* USB keyboard
* Keyboard martix
* USB joysticks
* VGA video (RGB222)
* LCD video (ST7789)
* PWM sound (1 pin)
* SPI SD card

By default, the board starts up using the LCD as its display.
To boot into VGA mode hold down the 'fire' button during reset.
Placing vga.txt on the SD card in the zxspectrum folder changes the default to VGA.


While on the menu...
* SHIFT-Fire = ESC
* RELOAD = Backspace
* SHIFT-RELOAD = Del

#### Circuit Diagram
![image](ZxSpectrumPicomputerZxLcd.png)

#### Firmware
[ZxSpectrumPicomputerZxLcd.uf2](/uf2/ZxSpectrumPicomputerZxLcd.uf2)

#### Build system notes
The LCD firmware has some switches to adapt it to different LCD panels:
| Symbol | Description | Default in code | PICOZX | ILI19341 |
| INVERSE_LCD | invert the brightness of the RGB color components | undefined | undefined | defined |
| LCD_MIRROR_X | Mirror the LCD output horizontally | undefined | defined | undefined |
| LCD_RGB | Use RBG order rather than BGR | undefined | undefined |  defined |


#### References
[hackster](https://www.hackster.io/news/peter-bobricius-misenko-s-picozx-lcd-is-the-handheld-sinclair-zx-spectrum-the-1980s-never-got-24c00f395b9d)<br/>
[raspberrypi.com](https://www.raspberrypi.com/news/build-your-own-handheld-zx-spectrum-with-raspberry-pi-pico/)


