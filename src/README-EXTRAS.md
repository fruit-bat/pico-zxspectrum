IMPORTANT NOTE
=

The links on the `README.md` page to the `.uf2` files are not in the extras repository. You must build from source,
but use this `pico-spectrum` repository when cloning in the instructions.

EXTRAS
=

  * Added registers `14` and `15` to `AY` sound chip as a tick counter reset on `IRQ`.
  * Made `IM 3` be `ED 4E` enter into an architecture extension mode.
  * Made `IM 0` an `ED 46` prefix for architecture extensions in the `IM 3` mode.

STILL TODO
=

  * The Jupiter Ace was a Z80 based machine, so a replacement of `128k_rom_1.h` (ROM0) as a forth boot leaving the `128k_rom_2.h` (ROM1) as the BASIC ROM. Needs `sudo apt install hxtools z80asm`.
  * Still some `ED (Misc)` opcode space available.

POSSIBLE VIDEO EXTENSION
=

A video resolution in 8 colours at `256 * 168` is possible as 3 bit-planes of 5 kB each.
