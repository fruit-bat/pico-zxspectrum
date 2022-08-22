IMPORTANT NOTE
=

The links on the `README.md` page to the `.uf2` files are not in the extras repository. You must build from source,
but use this `pico-spectrum` repository when cloning in the instructions.

EXTRAS
=

  * IM2 mode uses a fixed `data_on_bus` of `0xff` and uses `(i<<8+0xff)` for the vector.
  * Made `IM 0` an `ED 46` prefix for own use.

STILL TODO
=

  * The Jupiter Ace was a Z80 based machine, so a replacement of `128k_rom_1.h` (ROM0) as a forth boot leaving the `128k_rom_2.h` (ROM1) as the BASIC ROM. Needs `sudo apt install hxtools z80asm`.
  * Add extra features to AY sound chip. Two extra registers are available.
  * Still some `DD (IX)` and `FD (IY)` opcode space available.
  * Still some `ED (Misc)` opcode space available.
