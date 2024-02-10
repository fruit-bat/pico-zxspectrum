### ZxSpectrumPicomputerMax
These is the target for Bobricius' Retro PICOmputerMAX.

<img src="picomputermax.png" height="250"/>

It supports the following:
* USB keyboard
* USB joysticks
* Matrix keyboard (6x6)
* LCD video (ST7789)
* PWM sound (1 pin)
* SPI SD card

#### Circuit diagrams

![image](ZxSpectrumPicomputerMax.png)

#### Firmware

[ZxSpectrumPicocomputerMax.uf2](/uf2/ZxSpectrumPicocomputerMax.uf2)

#### Keyboard mappings
Trying to squeeze in all the key mappings is tricky but here is an attempt.

These are the nomal key mappings:<br/>
<img src="retro_vga_keyboard_normal.svg" width="500"/><br/>

These are the mappings with the ALt key down.:<br/>
<img src="retro_vga_keyboard_alt_down.svg" width="500"/><br/>
Shifted and numeric mappings are turned on and off using the arrow keys (up, down, left, right).
QS1, QS2, ...  save the emulator state to the appropriate Quick Save slot.

If there is a save in QS1 it will load after the emulator is powered-on or reset.

The SN keys load snapshots as if they are in a loop. 
SN loads the current snapshot, SN- load the previous snapshot and SN+ loads the next snapshot.

These are the mappings with the numeric shift on:<br/>
<img src="retro_vga_keyboard_num.svg" width="500"/><br/>

Alt+V sets the arrow keys to behave like a Kempston joystick,
Alt+C sets the arrow keys to operate the Spectrum cursor keys.

#### Kiosk mode
Kiosk mode disables the menu system and quick-save buttons. 
Kiosk mode is enabled by placing the following file on the SD-card:
```bash
zxspectrum/kiosk.txt
```

#### References

[Hackaday](https://hackaday.io/project/183398-retrovga-raspbery-pico-multi-retro-computer)