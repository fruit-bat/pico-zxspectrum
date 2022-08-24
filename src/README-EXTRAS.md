IMPORTANT NOTE
=

The links on the `README.md` page to the `.uf2` files are not in the extras repository. You must build from source,
but use this `pico-spectrum` repository when cloning in the instructions.

EXTRAS
=

  * Added registers `14` and `15` to `AY-3-8912` sound chip as a tick counter reset on `IRQ`. This follows the general endian order of the frequencies. These supposedly were serial bit bang interfaces but I think the `AY-3-8913` was better and pin `16` could have been an `IRQ` input.
  * Made `IM 3` be an (`ED 4E` enter into an ISA extension) extension mode. It also is a direct on a page boundary vectored by `I` interrupt mode.
    * Made `IM 0` an `ED 46` prefix for ISA extensions in the `IM 3` mode. These are different from `ED xx` as they may include extra registered information needing to be saved, and not just useful but keeping the Z80 basics. 

STILL TODO
=

  * The Jupiter Ace was a Z80 based machine, so a replacement of `128k_rom_1.h` (ROM0) as a forth boot leaving the `128k_rom_2.h` (ROM1) as the BASIC ROM. Needs `sudo apt install hxtools z80asm`.
  * Still some `ED (Misc)` opcode space available.

POSSIBLE VIDEO EXTENSION
=

A video resolution in 8 colours at `256 * 168` is possible as 3 bit-planes of 5 kB each.
