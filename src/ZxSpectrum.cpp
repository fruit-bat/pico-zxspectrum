#include "ZxSpectrum.h"
#include "basic.h"

#include <string.h>
#include <pico/printf.h>
#include <pico/stdlib.h>
#include "SizingOutputStream.h"

ZxSpectrum::ZxSpectrum(
    ZxSpectrumKeyboard *keyboard1,
    ZxSpectrumKeyboard *keyboard2,
    ZxSpectrumJoystick *joystick
) :
  _moderate(9),
  _keyboard1(keyboard1),
  _keyboard2(keyboard2),
  _joystick(joystick),
  _borderColour(7),
  _port254(0),
  _ear(false),
  _earInvert(0),
  _earDc(0),
  _buzzer(0)
{
  z80Power(true);
  _Z80.context = this;
  _Z80.options = Z80_MODEL_ZILOG_NMOS;
  _Z80.read = readByte;
  _Z80.fetch = readByte;
  _Z80.fetch_opcode = readByte;
  _Z80.write = writeByte;
  _Z80.in = readIO;
  _Z80.out = writeIO;
  _Z80.inta = readInt;
  reset(ZxSpectrum128k);
}

void ZxSpectrum::transmute(ZxSpectrumType type) {
  _type = type;
  switch (type) {
    case ZxSpectrum48k:
      setPageaddr(0, (uint8_t*)basic);
      _portMem = 0x20;
      break;
    case ZxSpectrum128k:
    default:
      setPageaddr(0, (uint8_t*)zx_128k_rom_1);
      _portMem = 0;
      break;
  }
}

void ZxSpectrum::reset(ZxSpectrumType type)
{
  _type = type;
  z80Reset();
  _ta32 = 0;
  _earInvert = 0;
  _earDc = 0;
  moderate(9);
  transmute(type);
  setPageaddr(1, (uint8_t*)&_RAM[5]);
  setPageaddr(2, (uint8_t*)&_RAM[2]);
  setPageaddr(3, (uint8_t*)&_RAM[0]);
  _ear = false;
  _pulseChain.reset();
  memset(_RAM, 0, sizeof(_RAM));
  _port254 = 0x20;  //0x30;
  _ay.reset();
  if (_keyboard1) _keyboard1->reset();
  if (_keyboard2) _keyboard2->reset();
  _tu32 = time_us_32() << 5;
}

// 0 - Z80 unmoderated
// 8 - Z80 at 4.0Mhz
// 9 - Z80 at 3.5Mhz
void ZxSpectrum::moderate(uint32_t mul) {
  if (mul == _moderate) return;
  if (mul) {
    _tu32 = time_us_32() << 5;
    _ta32 = 0;
  }
  _moderate = mul;
}

void ZxSpectrum::toggleModerate() {
  switch(_moderate) {
    case 9: moderate(8); break;
    case 8: moderate(0); break;
    case 0: moderate(9); break;
    default: break;
  }
}

int32_t ZxSpectrum::writeZ80(OutputStream *os, int32_t version) {
  int32_t r = writeZ80Header(os, version);
  if (r < 0) {
    DBG_SPEC("Failed to write Z80 header\n");
    return r;
  }
  switch(version) {
    case 0: {
      r = writeZ80MemV0(os);
      break;
    }
    case 1: {
      r = writeZ80MemV1(os);
      break;
    }
    case 3: {
      r = writeZ80HeaderV3(os);
      if (r < 0) {
        DBG_SPEC("Failed to write Z80 header V3\n");
        return r;
      }
      r = writeZ80MemV2(os);
      if (r < 0) {
        DBG_SPEC("Failed to write Z80 memory V2\n");
        return r;
      }         
      break;
    }
    default: {
      DBG_SPEC("Writing Z80 version %ld is not implemented!\n", version);
      r = -2;
      break;
    }
  }
  os->close();
  return r;
}

int32_t ZxSpectrum::writeZ80MemV2(OutputStream *os) {
  for (uint32_t b = 0; b < 12; ++b) {
    const int32_t p = z80BlockToPage(b, _type);
    if (p >=0) {
      int32_t r = writeZ80MemV2(os, b, p);
      if (r < 0) {
        DBG_SPEC("Failed to write Z80 V2 block %ld\n", b);
        return r;
      }
    }
  }
  return 0;
}

int32_t ZxSpectrum::writeZ80MemV2(OutputStream *os, const uint8_t b, const uint8_t p) {
  uint8_t *blk = (uint8_t *)&_RAM[p];
  SizingOutputStream sos;
  int32_t r = writeZ80MemV2(&sos, blk);
  if (r < 0) return r;
  uint32_t l = sos.length();
  r = os->writeWord(l);
  if (r < 0) return r;
  r = os->writeByte(b);
  if (r < 0) return r;
  return writeZ80MemV2(os, blk);
}

int32_t ZxSpectrum::writeZ80MemV2(OutputStream *os, uint8_t *blk) {
  unsigned char buf[4];
  int32_t l = -1;
  int32_t c = 0;
  for (uint32_t i = 0; i < 0x4000; ++i) {
    int32_t b = blk[i];
    if (b == l) {
      c++;
      if (c == 0xff || i == 0x3fff || l == 0xed) {
        buf[0] = 0xed;
        buf[1] = 0xed;
        buf[2] = c;
        buf[3] = l;
        int32_t r = os->write(buf, 4);
        if (r < 0) return r;
        l = -1;
        c = 0;
      }
    }
    else {
      if (c > 4) {
        buf[0] = 0xed;
        buf[1] = 0xed;
        buf[2] = c;
        buf[3] = l;
        int32_t r = os->write(buf, 4);
        if (r < 0) return r;
      }
      else if (c > 0) {
        buf[0] = l;
        buf[1] = l;
        buf[2] = l;
        buf[3] = l;
        int32_t r = os->write(buf, c);
        if (r < 0) return r;
      }

      if (l == 0xed || i == 0x3fff) {
        int32_t r = os->writeByte(b);
        if (r < 0) return r;
        l = -1;
        c = 0;
      }
      else {
        l = b;
        c = 1;
      }
    }
  }
  return 0;
}

int32_t ZxSpectrum::writeZ80MemV1(OutputStream *os) {
  unsigned char buf[4];
  int32_t l = -1;
  int32_t c = 0;
  for (uint32_t i = 0x4000; i < 0x10000; ++i) {
    int32_t b = readByte(i);
    if (b == l) {
      c++;
      if (c == 0xff || i == 0xffff || l == 0xed) {
        buf[0] = 0xed;
        buf[1] = 0xed;
        buf[2] = c;
        buf[3] = l;
        int32_t r = os->write(buf, 4);
        if (r < 0) return r;
        l = -1;
        c = 0;
      }
    }
    else {
      if (c > 4) {
        buf[0] = 0xed;
        buf[1] = 0xed;
        buf[2] = c;
        buf[3] = l;
        int32_t r = os->write(buf, 4);
        if (r < 0) return r;
      }
      else if (c > 0) {
        buf[0] = l;
        buf[1] = l;
        buf[2] = l;
        buf[3] = l;
        int32_t r = os->write(buf, c);
        if (r < 0) return r;
      }

      if (l == 0xed || i == 0xffff) {
        int32_t r = os->writeByte(b);
        if (r < 0) return r;
        l = -1;
        c = 0;
      }
      else {
        l = b;
        c = 1;
      }
    }
  }
  return 0;
}

int32_t ZxSpectrum::writeZ80MemV0(OutputStream *os) {
  for (uint32_t i = 0x4000; i < 0x10000; ++i) {
    int32_t b = readByte(i);
    int32_t r = os->writeByte(b);
    if (r < 0) return r;
  }
  return 0;
}

int32_t ZxSpectrum::writeZ80Header(
  OutputStream *os,
  int32_t version
) {
  bool samRom = false; // TODO What is this ! ??
  bool compresed = version == 1;
  unsigned char buf[30];

  buf[0] = Z80_A(_Z80);
  buf[1] = Z80_F(_Z80);
  buf[2] = Z80_C(_Z80);
  buf[3] = Z80_B(_Z80);
  buf[4] = Z80_L(_Z80);
  buf[5] = Z80_H(_Z80);
  buf[6] = version > 1 ? 0 : Z80_PCL(_Z80);
  buf[7] = version > 1 ? 0 : Z80_PCH(_Z80);
  buf[8] = Z80_SPL(_Z80);
  buf[9] = Z80_SPH(_Z80);
  buf[10] = _Z80.i;
  buf[11] = _Z80.r;
  buf[12] = (
    (_Z80.r >> 7) |
    (_borderColour << 1) |
    ((samRom ? 1 : 0) << 4) |
    ((compresed ? 1 : 0) << 5)
  );
  buf[13] = Z80_E(_Z80);
  buf[14] = Z80_D(_Z80);
  buf[15] = Z80_C_(_Z80);
  buf[16] = Z80_B_(_Z80);
  buf[17] = Z80_E_(_Z80);
  buf[18] = Z80_D_(_Z80);
  buf[19] = Z80_L_(_Z80);
  buf[20] = Z80_H_(_Z80);
  buf[21] = Z80_A_(_Z80);
  buf[22] = Z80_F_(_Z80);
  buf[23] = Z80_IYL(_Z80);
  buf[24] = Z80_IYH(_Z80);
  buf[25] = Z80_IXL(_Z80);
  buf[26] = Z80_IXH(_Z80);
  buf[27] = _Z80.iff1;
  buf[28] = _Z80.iff2;
  buf[29] = _Z80.im;

  return os->write(buf, 30);
}

int32_t ZxSpectrum::loadZ80Header(InputStream *is) {
/*
        Offset  Length  Description
        ---------------------------
        0       1       A register
        1       1       F register
        2       2       BC register pair (LSB, i.e. C, first)
        4       2       HL register pair
        6       2       Program counter
        8       2       Stack pointer
        10      1       Interrupt register
        11      1       Refresh register (Bit 7 is not significant!)
        12      1       Bit 0  : Bit 7 of the R-register
                        Bit 1-3: Border colour
                        Bit 4  : 1=Basic SamRom switched in
                        Bit 5  : 1=Block of data is compressed
                        Bit 6-7: No meaning
        13      2       DE register pair
        15      2       BC' register pair
        17      2       DE' register pair
        19      2       HL' register pair
        21      1       A' register
        22      1       F' register
        23      2       IY register (Again LSB first)
        25      2       IX register
        27      1       Interrupt flipflop, 0=DI, otherwise EI
        28      1       IFF2 (not particularly important...)
        29      1       Bit 0-1: Interrupt mode (0, 1 or 2)
                        Bit 2  : 1=Issue 2 emulation
                        Bit 3  : 1=Double interrupt frequency
                        Bit 4-5: 1=High video synchronisation
                                 3=Low video synchronisation
                                 0,2=Normal
                        Bit 6-7: 0=Cursor/Protek/AGF joystick
                                 1=Kempston joystick
                                 2=Sinclair 2 Left joystick (or user
                                   defined, for version 3 .z80 files)
                                 3=Sinclair 2 Right joystick
*/
  DBG_SPEC("Reading Z80 header\n");
  unsigned char buf[30];
  int32_t l = is->read(buf, sizeof(buf));
  if (l < 30) {
    DBG_SPEC("Failed to read Z80 header\n");
    return -2; // error
  }
  if (buf[12] == 255) buf[12] = 1;
  uint32_t pc = ((uint32_t)buf[6]) + (((uint32_t)buf[7]) << 8);
  bool compressed = (1 << 5) & buf[12];
  Z80_A(_Z80) = buf[0];
  Z80_F(_Z80) = buf[1];
  Z80_C(_Z80) = buf[2];
  Z80_B(_Z80) = buf[3];
  Z80_L(_Z80) = buf[4];
  Z80_H(_Z80) = buf[5];
  Z80_PCL(_Z80) = buf[6];
  Z80_PCH(_Z80) = buf[7];
  Z80_SPL(_Z80) = buf[8];
  Z80_SPH(_Z80) = buf[9];
  _Z80.i = buf[10];
  _Z80.r = (buf[11] & 0x7f) + ((buf[12] & 1) << 7);
  _borderColour = (buf[12] & (7 << 1)) >> 1;
  Z80_E(_Z80) = buf[13];
  Z80_D(_Z80) = buf[14];
  Z80_C_(_Z80) = buf[15];
  Z80_B_(_Z80) = buf[16];
  Z80_E_(_Z80) = buf[17];
  Z80_D_(_Z80) = buf[18];
  Z80_L_(_Z80) = buf[19];
  Z80_H_(_Z80) = buf[20];
  Z80_A_(_Z80) = buf[21];
  Z80_F_(_Z80) = buf[22];
  Z80_IYL(_Z80) = buf[23];
  Z80_IYH(_Z80) = buf[24];
  Z80_IXL(_Z80) = buf[25];
  Z80_IXH(_Z80) = buf[26];
  _Z80.iff1 = buf[27] ? 1 : 0;
  _Z80.iff2 = buf[28] ? 1 : 0;
  _Z80.im = buf[29] & 3;
  if (buf[12] & (1<<4)) DBG_SPEC("WARNING: SamRom enabled\n");
  DBG_SPEC("PC %04lX\n", pc);
  DBG_SPEC("IFF1 %02X\n", buf[27]);
  DBG_SPEC("IFF2 %02X\n", buf[28]);
  DBG_SPEC("IM %02X\n", buf[29] & 3);
  DBG_SPEC("Joystick %02X\n", buf[29] >> 6);
  DBG_SPEC("Applied Z80 header\n");

  Z80_MEMPTR(_Z80) = Z80_PC(_Z80);
  return pc == 0 ? 2 : compressed ? 1 : 0;
}

int32_t ZxSpectrum::writeZ80HeaderV3(OutputStream *os) {
  uint8_t buf[57];
  buf[30 - 30] = (sizeof(buf)-2) & 0xff;
  buf[31 - 30] = (sizeof(buf)-2) >> 8;
  buf[32 - 30] = Z80_PCL(_Z80);
  buf[33 - 30] = Z80_PCH(_Z80);
  Z80_MEMPTR(_Z80) = Z80_PC(_Z80);
  switch(_type) {
    case ZxSpectrum48k: {
      buf[34 - 30] = 0;
      buf[35 - 30] = 255;
      break;
    }
    case ZxSpectrum128k: {
      buf[34 - 30] = 4;
      buf[35 - 30] = _portMem;
      break;
    }
    default:
      DBG_SPEC("Invalid machine type to write to Z80 v3 header %d\n", _type);
      return -2;
  }
  buf[36 - 30] = 255;
  buf[37 - 30] = 0x04;
  buf[38 - 30] = _ay.readCtrl();
  for (uint8_t i = 0; i < 16; ++i) buf[39 - 30 + i] = _ay.readData(i);
  buf[55 - 30] = 0;
  buf[56 - 30] = 0;
  buf[57 - 30] = 0;
  buf[58 - 30] = 0;
  buf[59 - 30] = 0;
  buf[60 - 30] = 0;
  buf[61 - 30] = 0;
  buf[62 - 30] = 0;
  for (uint8_t i = 0; i < 10; ++i) buf[63 - 30 + i] = 0;
  for (uint8_t i = 0; i < 10; ++i) buf[73 - 30 + i] = 0;
  buf[84 - 30] = 0;
  buf[85 - 30] = 0;
  buf[86 - 30] = 255;
  return os->write(buf, sizeof(buf));
}

int32_t ZxSpectrum::loadZ80HeaderV2(InputStream *is, ZxSpectrumType *type) {
/*
        Offset  Length  Description
        ---------------------------
      * 30      2       Length of additional header block (see below)
      * 32      2       Program counter
      * 34      1       Hardware mode (see below)
      * 35      1       If in SamRam mode, bitwise state of 74ls259.
                        For example, bit 6=1 after an OUT 31,13 (=2*6+1)
                        If in 128 mode, contains last OUT to 0x7ffd
	                      If in Timex mode, contains last OUT to 0xf4
      * 36      1       Contains 0xff if Interface I rom paged
                        If in Timex mode, contains last OUT to 0xff
      * 37      1       Bit 0: 1 if R register emulation on
                        Bit 1: 1 if LDIR emulation on
                        Bit 2: AY sound in use, even on 48K machines
                        Bit 6: (if bit 2 set) Fuller Audio Box emulation
                        Bit 7: Modify hardware (see below)
      * 38      1       Last OUT to port 0xfffd (soundchip register number)
      * 39      16      Contents of the sound chip registers
        55      2       Low T state counter
        57      1       Hi T state counter
        58      1       Flag byte used by Spectator (QL spec. emulator)
                        Ignored by Z80 when loading, zero when saving
        59      1       0xff if MGT Rom paged
        60      1       0xff if Multiface Rom paged. Should always be 0.
        61      1       0xff if 0-8191 is ROM, 0 if RAM
        62      1       0xff if 8192-16383 is ROM, 0 if RAM
        63      10      5 x keyboard mappings for user defined joystick
        73      10      5 x ASCII word: keys corresponding to mappings above
        83      1       MGT type: 0=Disciple+Epson,1=Disciple+HP,16=Plus D
        84      1       Disciple inhibit button status: 0=out, 0ff=in
        85      1       Disciple inhibit flag: 0=rom pageable, 0ff=not
     ** 86      1       Last OUT to port 0x1ffd
*/
  uint8_t buf[57];
  const int32_t hl = is->readWord();
  if (hl > 57) {
    DBG_SPEC("Invalud Z80 V2/V3 header length %ld\n", hl);
    return -2; // error
  }
  int32_t l = is->read(buf, hl);
  if (l < hl) {
    DBG_SPEC("Failed to read Z80 V2 header\n");
    return -2; // error
  }
  Z80_PCL(_Z80) = buf[0];
  Z80_PCH(_Z80) = buf[1];
  const int32_t v = (hl >=54) ? 3 : 2;
/*
        Value:          Meaning in v2           Meaning in v3
      -----------------------------------------------------
        0             48k                     48k
        1             48k + If.1              48k + If.1
        2             SamRam                  SamRam
        3             128k                    48k + M.G.T.
        4             128k + If.1             128k
        5             -                       128k + If.1
        6             -                       128k + M.G.T.
*/ 
  const int32_t hm = buf[34-32];
  *type = (hm == 0) || (hm == 1) || ((hm == 3) && (v == 3)) ? ZxSpectrum48k : ZxSpectrum128k;
  DBG_SPEC("Found header for V%ld and hardware mode %ld is48k %s\n", v, hm, (*type == ZxSpectrum48k ? "yes" : "no"));
  transmute(*type);
  _port254 = 0x20;  //0x30;
  if (*type == ZxSpectrum128k) {
    _portMem = buf[35-32];
    setPageaddr(3, (uint8_t*)&_RAM[_portMem & 7]);
    setPageaddr(0, (uint8_t*)((_portMem & 0x10) ? zx_128k_rom_2 : zx_128k_rom_1));
  }
  for (int32_t i = 0; i < 16; ++i) {
    _ay.writeCtrl(i);
    _ay.writeData(buf[i + 39-32]);
  }
  _ay.writeCtrl(buf[38-32]);
  return 0;
}

/*
        Block   In '48 mode     In '128 mode    In SamRam mode
        ------------------------------------------------------
         0      48K rom         rom (basic)     48K rom
         1      Interface I, Disciple or Plus D rom, according to setting
         2      -               rom (reset)     samram rom (basic)
         3      -               page 0          samram rom (monitor,..)
         4      8000-bfff       page 1          Normal 8000-bfff
         5      c000-ffff       page 2          Normal c000-ffff
         6      -               page 3          Shadow 8000-bfff
         7      -               page 4          Shadow c000-ffff
         8      4000-7fff       page 5          4000-7fff
         9      -               page 6          -
        10      -               page 7          -
        11      Multiface rom   Multiface rom   -
*/
int32_t ZxSpectrum::z80BlockToPage(const uint8_t b, const ZxSpectrumType type) {
  //                                 0   1   2   3   4   5   6   7   8   9  10  11
  const static int8_t z80p48k[]  = {-1, -1, -1, -1,  2,  0, -1, -1,  5, -1, -1, -1};
  const static int8_t z80p128k[] = {-1, -1, -1,  0,  1,  2,  3,  4,  5,  6,  7, -1};

  int32_t p = -1;
  switch(type) {
    case ZxSpectrum48k:  p = z80p48k[b];  break;
    case ZxSpectrum128k: p = z80p128k[b]; break;
    default: break;
  }  
  return p;
}

/*
    Byte    Length  Description
    ---------------------------
    0       2       Length of compressed data (without this 3-byte header)
                    If length=0xffff, data is 16384 bytes long and not compressed
    2       1       Page number of block
    3       [0]     Data
*/   
int32_t ZxSpectrum::loadZ80MemBlock(InputStream *is, const ZxSpectrumType type) {
 
  int32_t l = is->readWord();
  if (l < 0) return l;
  DBG_SPEC("Found Z80 V2 block of length %ld\n", l);
  int32_t b = is->readByte();
  if (b < 0) {
    DBG_SPEC("Failed to read Z80 V2 block number\n");
    return -2;
  }
  DBG_SPEC("Found Z80 V2 block %ld\n", b);

  if (b >= 11) {
    DBG_SPEC("Invalid Z80 V2 block number %ld\n", b);
    return -2;
  }

  int32_t p = z80BlockToPage(b, type);
  
  if (p == -1) {
      DBG_SPEC("Failed to translate Z80 V2 block %ld\n", b);
      return -2;
  }
  
  uint8_t *m = (uint8_t *)&_RAM[p];
  if (l == 0xffff) {
    int32_t r = is->read(m, 1<<14);
    if (r < 1<<14) {
      DBG_SPEC("Failed to read uncompressed data for Z80 V2 block %ld\n", b);
      return -2;
    }
  }
  else {
    int32_t s = l;
    uint32_t i = 0x0000;
    uint32_t wd = 0;
    uint32_t wf = 0;
    while (i < 0x4000) {
      int32_t r;
      if (s > 0) { r = is->readByte(); s--; } else { r = -1; }
      if (r < -1) return r;
      wd <<= 8; wf <<= 1;
      if (r >= 0) { wd |= r; wf |= 1; }
      if ((wf & 0xf) == 0) break;
      if ((wf & 0xf) == 0xf) {
        if ((wd & 0xffff0000) == 0xeded0000) {
          const uint32_t d = wd & 0xff;
          const uint32_t e = i + ((wd >> 8) & 0xff);
          const uint32_t f = e > 0x4000 ? 0x4000 : e;
          while (i < f) m[i++] = d;
          wf = 0;
        }
      }
      if (wf & 0x8) {
        m[i++] = wd >> 24;
      }
    }
    DBG_SPEC("read %ld compressed block bytes %ld under-run\n", i, s);
  }
  return 0;
}

int32_t ZxSpectrum::loadZ80MemV0(InputStream *is) {
  transmute(ZxSpectrum48k);
  for (uint32_t i = 0x4000; i < 0x10000; ++i) {
    int32_t r = is->readByte();
    if (r < 0) return r;
    writeByte(i, r);
  }
  return -1;
}

int32_t ZxSpectrum::loadZ80MemV1(InputStream *is) {
  transmute(ZxSpectrum48k);
  uint32_t i = 0x4000;
  uint32_t wd = 0;
  uint32_t wf = 0;
  while (i < 0x10000) {
    int32_t r = is->readByte();
    if (r < -1) return r;
    wd <<= 8; wf <<= 1;
    if (r >= 0) { wd |= r; wf |= 1; }
    if ((wf & 0xf) == 0) break;
    if ((wf & 0xf) == 0xf) {
      if (wd == 0x00eded00) break;
      if ((wd & 0xffff0000) == 0xeded0000) {
        const uint32_t d = wd & 0xff;
        const uint32_t e = i + ((wd >> 8) & 0xff);
        const uint32_t f = e > 0x10000 ? 0x10000 : e;
        while (i < f) writeByte(i++, d);
        wf = 0;
      }
    }
    if (wf & 0x8) {
      writeByte(i++, wd >> 24);
    }
  }
  return i - 0x4000;
}

void ZxSpectrum::loadZ80(InputStream *is) {
  reset(ZxSpectrum128k);
  int32_t version = loadZ80Header(is);
  DBG_SPEC("Reading Z80 version %ld\n", version);
  if (version >= 0) {
    switch(version) {
      case 0:
        loadZ80MemV0(is);
        break;
      case 1:
        loadZ80MemV1(is);
        break;
      case 2:
        ZxSpectrumType type;
        if (loadZ80HeaderV2(is, &type) >= 0) {
          while(loadZ80MemBlock(is, type) >= 0);
        }
        break;
      default:
        DBG_SPEC("Reading Z80 version %ld is not implemented!\n", version);
        break;
    }
  }
  is->close();
}

void ZxSpectrum::saveZ80(OutputStream *os) {
  writeZ80(os, 3);
}

uint32_t ZxSpectrum::tStatesPerMilliSecond() {
  return _moderate > 0 ? (32000 / _moderate) : 4000;
}

void ZxSpectrum::loadTap(InputStream *inputStream) {
  _pulseChain.loadTap(
    inputStream,
    tStatesPerMilliSecond()
  );
}

void ZxSpectrum::loadTzx(InputStream *inputStream) {
  _pulseChain.loadTzx(
    inputStream,
    tStatesPerMilliSecond(),
    _type == ZxSpectrum48k
  ); 
}

void ZxSpectrum::ejectTape() {
  _pulseChain.reset();
}

void ZxSpectrum::pauseTape(bool pause) {
  _pulseChain.pause(pause);
}

void ZxSpectrum::togglePauseTape() {
  _pulseChain.pause(!_pulseChain.paused());
}

bool ZxSpectrum::tapePaused() {
  return !_pulseChain.playing();
}

