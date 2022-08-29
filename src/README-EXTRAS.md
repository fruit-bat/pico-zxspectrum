IMPORTANT NOTE
=

The links on the `README.md` page to the `.uf2` files are not in the extras repository. You must build from source,
but use this `pico-spectrum` repository when cloning in the instructions.

EXTRAS In this repository
=

  * Added registers `14` and `15` to `AY-3-8912` sound chip as a tick counter reset on `IRQ`. This follows the general endian order of the frequencies. These supposedly were serial bit bang interfaces but I think the `AY-3-8913` was better and pin `16` could have been an `IRQ` sync counter input.
  * Made `IM 3` be an (`ED 4E` enter into an ISA extension) extension mode. It also is a direct on a page boundary vectored by `I` interrupt mode. I think the mnemonic is `ZX`.
    * Made `IM 0` an `ED 46` prefix for ISA extensions in the `IM 3` mode. These are different from `ED xx` as they may include extra registered information needing to be saved, and not just useful but keeping the Z80 basics.
    * Made an IO port register at `0x3ffd` in hex. This is stored in the save without consuming extra formatting at the cost of one `IRQ` loss of sync. Register `15` of the `AY-3-8912` say yeah! A write followed by a non-zero check will confirm sync.
  * A video resolution in 8 colours at `256 * 128` is possible as 3 bit-planes of 4 kB each. This involved making some artificial colour attribute tables and using them with adjustments of the `screenPtr` to index alternates for each primary colour.
    * Use bit zero of port `0x3ffd` set making a 3 planes going in `Blue`, `Red` then `Green` order with increasing in memory address. They are contiguous planes and take 12 kB bytes of the 16 kB block of video memory.
    * Also added an alpha quality beam following bus mode, which places `0xff` or the first colour attribute of pixel data per line. This may allow sync on attributes by checking this alternate to floating port.
  * Added many new instructions to the Z80, mainly in the `ED xx` group. Although `DF ED` and `FF ED` do an `ex hl, ix/iy` respectively.
    * `ED 40` to `ED BF` completed and apart from errors should remain fixed in meaning. They could be seen as useful for indexing and arithmetic, with a few ZX Spectrum specials covering the screen layout. 

PORT `0x3ffd` Extras control
=

The port is arranged as follows

  * Bit `0` - Set for `256 * 128` resolution `GRB` mode.
  * Bit `1` -
  * Bit `2` -
  * Bit `3` -
  * Bit `4` -
  * Bit `5` -
  * Bit `6` -
  * Bit `7` -  

STILL TO DO
=

  * The Jupiter Ace was a Z80 based machine, so a replacement of `128k_rom_1.h` (ROM0) as a forth boot leaving the `128k_rom_2.h` (ROM1) as the BASIC ROM. Needs `sudo apt install hxtools z80asm`.
  * Still some `ED (Misc)` opcode space available. I've got enough ideas to fill in about half of the `Misc` plane.
