
## Updates

28/12/24

* Added a build with CVBS 50Hz output - credit to [svofski](https://github.com/svofski)

27/12/24

* Additions for Murmulator 2 (only mono PWM audio for now)
* Vcore and Vsys displayed on menu
* PIO PWM mono audio for some builds

The move to PWM audio is primarily to support the RP2350, on which I have been having problems with the timer interrupts. There were also some stability issues on the RP2040 particularly with the 270Mhz overclock. Anyhow, I think the sound quality is much better using the PIO, probably because the delivery of samples is done in sync with the PWM frame. The down side is I am only doing mono for now; I have run out of PIO state-machines on the RP2040, but the RP2350 has an extra PIO block so I may have a go at stereo just for the RP2350.

23/11/24

* Better beeper sound quality (Mark II)

18/11/24

* Better beeper sound quality (Should improve most builds)

16/10/24

* Published a selection of RP2350 builds
* There are now two firmware folders uf2-rp2040 and uf2-rp2350-arm-s

If you have a board for which you would like a RP2350 build please raise an issue and I 
will try to add it.

29/09/24

* Removed dependency on modified TinyUSB library
* Unmodified pico-sdk can be now used
* New library for USB joystick/mouse/keyboard. See https://github.com/fruit-bat/pico-hid-host

18/09/24

* Added support for NES/SNES joypads on MURMULATOR firmware
* New library to support reading NES/SNES joypads. See https://github.com/fruit-bat/pico-nespad

24/08/24

* Fixed USB joystick issue
* New support for Xinput joysticks thanks to [@DPRCZ](https://github.com/DPRCZ)
* Minor improvements to menu navigation
* Joystick buttons 2 & 4 together to access/exit menu

18/08/24

* Fixed an issue with keyboard input in some programs.

17/08/24

* Move to new VGA renderer (SDK update seems to have broken the old one)
Note that pico-extras needs to be the version from my site (https://github.com/fruit-bat/pico-extras)

11/08/24

* In menu Spectrum keyboard to aid joystick use thanks to [@DPRCZ](https://github.com/DPRCZ)

04/08/24

* USB joystick can now be used for menu navigation thanks to [@DPRCZ](https://github.com/DPRCZ)

22/07/24

* Added support for USB mouse to act as joystick, with settings in the menu. Thanks to [@javavi](https://github.com/javavi)
* Fixed an issue at with SD card access on startup; startup should be quicker and more reliable. Thanks to [@LiCaNtRoPo-PCB](https://github.com/LiCaNtRoPo-PCB)

08/07/24

* Added support for Kempston Mouse thanks to [@javavi](https://github.com/javavi)

23/06/24

* Improvements for Kempston Joystick emulation thanks to [@javavi](https://github.com/javavi)
  
27/05/24

* Handle longer HID descriptors for USB joysticks

13/05/24

* CAPS-4 for page down and CAPS-9 for page up on rubber keyboard.
* Simple first letter matching on Snapshot and Tape selectors.

11/05/24

* Broader joystick/gamepad support (thanks to DPRCZ) 

21/04/24

* 720x576x50Hz option now available for VGA and HDMI builds
* Ringo support on LCD, VGA and HMDI firmware

14/04/24

Finally, some 50Hz display builds!

* Updates to HDMI firmware and Pimoroni Pico Demo VGA board:
  * Added support for Ringo's low res mode
  * 720x576x50Hz display mode

The patch to make Ringo looks to see if the display is flipping every 4 lines and if so it switches to doing so with precise timing. It's a hack, but Ringo is a nice game and I'm happy to see it working.

I'll try to add 50Hz modes to the remaing VGA boards when I get time.

01/04/24
* New firmware with audio over HDMI
* New interrupt mode based on CPU cycle count (now default)
* New menu item to toggle interrupt between CPU cycle count and VSYNC
* Improved PWM audio output (samples now buffered) 

Thanks to [ikjordan](https://github.com/ikjordan) for his audio additions to [PicoDVI](https://github.com/ikjordan/PicoDVI).

01/04/24
* New firmware with audio over HDMI
* New interrupt mode based on CPU cycle count (now default)
* New menu item to toggle interrupt between CPU cycle count and VSYNC
* Improved PWM audio output (samples now buffered) 

Thanks to [ikjordan](https://github.com/ikjordan) for his audio additions to [PicoDVI](https://github.com/ikjordan/PicoDVI).

18/02/24 
* Added firmware for the Waveshare RP2040 - PiZero
* Added option to save snapshots from the menu 

11/02/24 
* Start to separate out docs for each target
* Initial support for ILI9341

20/01/24 - Maintenance release of pre-built firmware: 
* All .uf2 files rebuilt (see the uf2 folder)
* Minor bug fixes
* Updated libraries (Z80 etc)
* Misc joystick fixes
  
I've moved to a new build machine, so let me know if there are any issues.
At some point, I will try and organise the spralling README file.
  
01/01/24 - Added support for bobricius <a href="docs/pico_zx48_128">Pico ZX48/128</a>

25/06/23 - Ability to persist volume control and joystick type (Sinclair/Kempston). The settings are persisted to .config file on the SD card and are loaded as the defaults on reset.
To save setting goto ''settings->save'' on the menu. 
I was hoping to save to flash memory but not got that to work as of yet.
        
04/06/23 - Add MURMULATOR platform, with thanks to [Javavi](https://github.com/javavi) 

03/06/23 - Don't freeze on startup if no SD card is present. Release v0.33

29/04/23 - Added new target for Bobricious PICOZX LCD + general release v0.32
* PICOZX with LCD can now use either the LCD or VGA output
* Volume control (for target where it makes sense)
* Some changes to key mappings and menu navigation

To boot into VGA mode hold down the 'fire' button during reset.

The key mapping changes were intended to make navigating the menus with a joystick a bit easier.
In particular, now left (usually) goes 'back' and right enters/activates an item. 
Paging in the file-explorer is now achieved with the Page-Up and Page-Down keys.
Not sure it will stay like this; mapping keys and directions in a way that works for the emulator and the menus can be tricky.

19/01/23 - Added new target for [ArnoldUK](https://github.com/ArnoldUK).
This target can read from a standard 48k Spectrum keyboard matrix.

25/12/22 - Very basic support for sub-folders in the 'snapshots' and 'tapes' storage areas has been added.
Files can be renamed, copied and deleted from the menu system.
The quick-saves folder is now under the snapshots folder and behaves just like any other folder.
The file 'explorer' has the following commands:
* &lt;Enter&gt; - enter a folder/load a file
* ESC   - Go up a folder
* 1=DEL - Delete the file
* 2=REN - Rename the file
* 3=CPY - Copy the file
* 4=PST - Paste the file
* 5=REF - Reload the list of files in the current folder
* 6=SAV - Save a snapshot to the current folder

The function keys for playing tapes have been removed. 
The idea is that in the future more useful controls will be added like 'play', 'stop' and 'eject'.

5/12/22 - The emulator can now cope with more files in the snapshots and tapes folders without running out of memory.
I've tested up to 400 but it may survive more that this.
Directory entries are now written to a file .dcache which can be updated from the menus by 'rescanning' the folder.
If you are adding files to the folders on a PC you can just delete the .dcache and it will get regenerated on next power-on.

I've moved to the [Redcode Z80 emulator](https://github.com/redcode/Z80) as:
* It comes with a test suite
* It is much faster than the previous emulator

TZX support added with some omissions:
* No CSW support (raise an issue if this is important to you)
* There are some compatiblity issues

Builds with an RP_AUDIO_IN pin can now load from tape. 
Preparing the audio signal will require a little extra circuitry and some examples will be added to this page.

The move from [Carl's no-OS-FatFS-SD-SPI-RPi-Pico](https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico) to 
[Pimoroni's FatFS](https://github.com/pimoroni/pimoroni-pico) was made as the SD card pins on the 
[Pimoroni Pico DV Demo Base](https://shop.pimoroni.com/products/pimoroni-pico-dv-demo-base) do not match up with the
RP2040 SPI harware support. The Pimoroni library has a PIO SPI driver, which gets around the problem.