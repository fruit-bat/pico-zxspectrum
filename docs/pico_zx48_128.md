### PICO ZX48/128
This target matches the circuit board created by Bobricius.
It matches the original layout of the ZX Spectrum keyboard allowing it to be used with the old membranes.

<img src="pico_zx48_128_1.png" width="400"/>

#### Joystick buttons & modes
The PCB has some buttons which can act as a joystick or the ZX Spectrum cursor.
The LED on the Pico PI indicates the mode:

| LED | mode |
| - | - |
| OFF | Joystick mode |
| ON | Cursor mode |

To switch mode make sure you are viewing the ZX Spectrum display (not the menus),
then press CAPS SHIFT and one of  <<< or >>> at the same time:

| Keys | Mode |
| - | - |
| CAPS SHIFT <<< | Cursor |
| CAPS SHIFT >>> | Joystick |

When the machine boots, the buttons are configured to act as a joystick.

When in joystick mode, the buttons act as a Kemptson joystick.

#### Manu key mappings
These are Spectrum keyboard actions that can navigate the settings menu:

| keys conbination | Action |
| - | - |
| CAPS-4 | Page Down |
| CAPS-5 | Cursor Left |
| CAPS-6 | Cursor Down |
| CAPS-7 | Cursor Up |
| CAPS-8 | Cursor Right |
| CAPS-9 | Page Up |
| Enter | Perform action |

#### Circuit Diagrams
<img src="pico_zx48_128_2.png" width="400"/>

#### Firmware
| Display mode | Firmware |
| - | - |
| 720x576x50Hz | [ZxSpectrumPicomputerVgaAukBob_720x576x50Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerVgaAukBob_720x576x50Hz.uf2) |
| 640x480x60Hz | [ZxSpectrumPicomputerVgaAukBob_640x480x60Hz.uf2](/uf2-rp2040/ZxSpectrumPicomputerVgaAukBob_640x480x60Hz.uf2) |

#### References
[ZXRenew for cases and keyboards](https://zxrenew.co.uk/)<br/>
[YouTube 1](https://www.youtube.com/watch?v=BvZBZUznZrY)<br/>
[YouTube 2](https://www.youtube.com/watch?v=LgOR_wKjtFk)<br/>

