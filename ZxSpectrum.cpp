#include "ZxSpectrum.h"
#include "basic.h"
#include <memory.h>
#include <pico/printf.h>
#include <pico/stdlib.h>

ZxSpectrum::ZxSpectrum(
  ZxSpectrumKeyboard *keyboard
) : 
  _cycles(0),
  _moderate(false),
  _keyboard(keyboard),
  _borderColour(7),
  _RAM{{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}}
{
  _Z80.setCallbacks(this, readByte, writeByte, readWord, writeWord, readIO, writeIO);
}

void ZxSpectrum::reset(unsigned int address)
{
  _Z80.reset();
  _Z80.setPC(address);
  _cycles = 0;
  _tu4 = time_us_32() << 5;
  _ta4 = 0;
  setPageaddr(0, (unsigned char*)basic);
  setPageaddr(1, (unsigned char*)&_RAM[5]);
  setPageaddr(2, (unsigned char*)&_RAM[2]);
  setPageaddr(3, (unsigned char*)&_RAM[0]);
}

#define INSTRUCTION_PER_STEP 100

void ZxSpectrum::interrupt() {
  _Z80.IRQ(0x38);
}

void ZxSpectrum::step()
{
  for(int i=0; i < INSTRUCTION_PER_STEP; ++i) {
    int c = _Z80.step();
    _cycles += c;
    if (_moderate) {
      uint32_t tu4 = time_us_32() << 5;
      _ta4 += (c*9) - tu4 + _tu4; // +ve too fast, -ve too slow
      _tu4 = tu4;
      if (_ta4 > 32) busy_wait_us_32(_ta4 >> 5);
      if (_ta4 < -1000000) _ta4 = -1000000;
    }
  }
}

void ZxSpectrum::reset() { 
  reset(0x0000); 
}

void ZxSpectrum::moderate(bool on) {
  if (on == _moderate) return;
  
  if (on) {
    _tu4 = time_us_32() << 5;
    _ta4 = 0;
  }
  
  _moderate = on;
}

void ZxSpectrum::toggleModerate() {
  moderate(!_moderate);
}


int ZxSpectrum::writeZ80(OutputStream *os, int version) {
  int r = writeZ80Header(os, version);
  if (r < 0) return r;
  if (version > 1) {
    // TODO write the extra header
    printf("Writing Z80 version %d is not implemented!\n", version);
    return -2;
  }
  switch(version) {
    case 0:
      r = writeZ80MemV0(os);
      break;
    default:
      printf("Writing Z80 version %d is not implemented!\n", version);
      r = -2;
      break;      
  }
  
  return r;
}

int ZxSpectrum::writeZ80MemV0(OutputStream *os) {
  for (unsigned int i = 0x4000; i < 0x10000; ++i) {
    int b = readByte(i);
    int r = os->writeByte(b); 
    if (r < 0) return r;
  }
  return -1;
}

int ZxSpectrum::writeZ80Header(
  OutputStream *os,
  int version
) {
  bool samRom = true; // TODO What is this ! ??
  bool compresed = version == 1;
  unsigned char buf[30];

  buf[0] = _Z80.getA();
  buf[1] = _Z80.getF();
  buf[2] = _Z80.getC();
  buf[3] = _Z80.getB();
  buf[4] = _Z80.getL();
  buf[5] = _Z80.getH();
  buf[6] = version > 1 ? 0 : _Z80.getPC() & 0xff;
  buf[7] = version > 1 ? 0 : _Z80.getPC() >> 8;
  buf[8] = _Z80.getSPL();
  buf[9] = _Z80.getSPH(); 
  buf[10] = _Z80.getI();
  buf[11] = _Z80.getR();
  buf[12] = (
    (_Z80.getR() >> 7) |
    (_borderColour << 1) |
    ((samRom ? 1 : 0) << 4) |
    ((compresed ? 1 : 0) << 5)
  );
  buf[13] = _Z80.getE();
  buf[14] = _Z80.getD();
  _Z80.swap();
  buf[15] = _Z80.getC();
  buf[16] = _Z80.getB();
  buf[17] = _Z80.getE();
  buf[18] = _Z80.getD();
  buf[19] = _Z80.getL();
  buf[20] = _Z80.getH();
  buf[21] = _Z80.getA();
  buf[22] = _Z80.getF();  
  _Z80.swap();
  buf[23] = _Z80.getIYL();
  buf[24] = _Z80.getIYH();  
  buf[25] = _Z80.getIXL();
  buf[26] = _Z80.getIXH(); 
  buf[27] = _Z80.getIFF1();
  buf[28] = _Z80.getIFF2();
  buf[29] = _Z80.getIM();
  
  return os->write(buf, 30);
}

int ZxSpectrum::loadZ80Header(InputStream *is) {
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
  printf("Reading Z80 header\n");
  unsigned char buf[30];
  int l = is->read(buf, sizeof(buf));
  if (l < 30) { 
    printf("Failed to read Z80 header\n");
    return -2; // error
  }
  if (buf[12] == 255) buf[12] = 1;
  unsigned int pc = (unsigned int)buf[6] + (((unsigned int)buf[7]) << 8);
  bool compressed = (1 << 5) & buf[12];
  _Z80.setA(buf[0]);
  _Z80.setF(buf[1]);
  _Z80.setC(buf[2]);
  _Z80.setB(buf[3]);
  _Z80.setL(buf[4]);  
  _Z80.setH(buf[5]);
  _Z80.setPC(pc);
  _Z80.setSPL(buf[8]);
  _Z80.setSPH(buf[9]);
  _Z80.setI(buf[10]);
  _Z80.setR((buf[11] & 0x7f) + ((buf[12] & 1) << 7));
  _borderColour = (buf[12] & (7 << 1)) >> 1;
  _Z80.setE(buf[13]);
  _Z80.setD(buf[14]);
  _Z80.swap();
  _Z80.setC(buf[15]);
  _Z80.setB(buf[16]);
  _Z80.setE(buf[17]);
  _Z80.setD(buf[18]);
  _Z80.setL(buf[19]);  
  _Z80.setH(buf[20]);
  _Z80.setA(buf[21]);
  _Z80.setF(buf[22]);
  _Z80.swap();
  _Z80.setIYL(buf[23]);
  _Z80.setIYH(buf[24]);
  _Z80.setIXL(buf[25]);
  _Z80.setIXH(buf[26]);    
  _Z80.setIFF1(buf[27]);
  _Z80.setIFF2(buf[28]);
  _Z80.setIM(buf[29] & 3);
  
  printf("Applied Z80 header\n");

  return pc == 0 ? 2 : compressed ? 1 : 0;
}

int ZxSpectrum::loadZ80MemV0(InputStream *is) {
  for (unsigned int i = 0x4000; i < 0x10000; ++i) {
    int r = is->readByte(); 
    if (r < 0) return r;
    writeByte(i, r);
  }
  return -1;
}

int ZxSpectrum::loadZ80MemV1(InputStream *is) {
  unsigned int i = 0x4000;
  unsigned int wd = 0;
  unsigned int wf = 0;
  while (i < 0x10000) {
    int r = is->readByte(); 
    if (r < -1) return r;
    wd <<= 8; wf <<= 1;
    if (r >= 0) { wd |= r; wf |= 1; }
    if ((wf & 0xf) == 0) break;
    if ((wf & 0xf) == 0xf) {
      if (wd == 0x00eded00) break;
      if ((wd & 0xffff0000) == 0xeded0000) {
        const unsigned int d = wd & 0xff;
        const unsigned int e = i + ((wd >> 8) & 0xff);
        const unsigned int f = e > 0x10000 ? 0x10000 : e;
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
  reset();
  int version = loadZ80Header(is);
  printf("Reading Z80 version %d\n", version);
  if (version >= 0) {
    switch(version) {
      case 0:
        loadZ80MemV0(is);
        break;
      case 1:
        loadZ80MemV1(is);
        break;
      default:
        printf("Reading Z80 version %d is not implemented!\n", version);
        break;
    }
  }
  is->close();
}
