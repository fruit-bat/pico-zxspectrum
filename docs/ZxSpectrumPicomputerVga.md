

### ZxSpectrumPicomputerVga
This is the target for Bobricius' Retro VGA board:<br/>
<img src="retrovga.png" width="300"/>

It supports the following:
* USB keyboard
* USB joysticks
* Matrix keyboard (6x6)
* VGA video (RGB332)
* PWM sound (1 pin)
* SPI SD card

#### Circuit Diagram
![image](ZxSpectrumPicomputerVga.png)

#### Firmware
[ZxSpectrumPicocomputerVga.uf2](uf2/ZxSpectrumPicocomputerVga.uf2)

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