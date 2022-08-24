IMPORTANT NOTE
=

The links on the `README.md` page to the `.uf2` files are not in the extras repository. You must build from source,
but use this `pico-spectrum` repository when cloning in the instructions.

EXTRAS
=

  * Added registers `14` and `15` to `AY-3-8912` sound chip as a tick counter reset on `IRQ`. This follows the general endian order of the frequencies. These supposedly were serial bit bang interfaces but I think the `AY-3-8913` was better and pin `16` could have been an `IRQ` sync counter input.
  * Made `IM 3` be an (`ED 4E` enter into an ISA extension) extension mode. It also is a direct on a page boundary vectored by `I` interrupt mode.
    * Made `IM 0` an `ED 46` prefix for ISA extensions in the `IM 3` mode. These are different from `ED xx` as they may include extra registered information needing to be saved, and not just useful but keeping the Z80 basics.
    * Made an IO port register at `3ffd` in hex. This is stored in the save without consuming extra formatting at the cost of one `IRQ` loss of sync. Register `15` of the `AY-3-8912` say yeah!
  * A video resolution in 8 colours at `256 * 168` is possible as 3 bit-planes of about 5 kB each. Or 63 pages of 256 bytes. This involves making some artificial colour attribute tables and using them with some adjustments of the `screenPtr` to index alternates for each primary colour.
    * Use bit zero of port `3ffd` for set making a set of 3 planes going in B, R then G order with increasing in memory address. They are contiguous planes and a final free 256 bytes completes the 16 kB block of video memory.

STILL TODO
=

  * The Jupiter Ace was a Z80 based machine, so a replacement of `128k_rom_1.h` (ROM0) as a forth boot leaving the `128k_rom_2.h` (ROM1) as the BASIC ROM. Needs `sudo apt install hxtools z80asm`.
  * Still some `ED (Misc)` opcode space available. I've got enough ideas to fill in about half of the `Misc` plane.
