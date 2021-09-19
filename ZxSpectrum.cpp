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
  _RAM{0}
{
  _Z80.setCallbacks(this, readByte, writeByte, readWord, writeWord, readIO, writeIO);
  memcpy(&_RAM[0x0000], basic, 0x4000);
}

void ZxSpectrum::reset(unsigned int address)
{
  _Z80.reset();
  _Z80.setPC(address);
  _cycles = 0;
}

#define INSTRUCTION_PER_STEP 100

void ZxSpectrum::step()
{
  // printf("PC:%04X ", _Z80.getPC()); 
  for(int i=0; i < INSTRUCTION_PER_STEP; ++i) {
    int c = _Z80.step();
    _cycles += c;
    if (_moderate) {
      uint32_t tu4 = time_us_32() << 2;
      _ta4 += c - tu4 + _tu4; // +ve too fast, -ve too slow
      _tu4 = tu4;
      if (_ta4 > 4) busy_wait_us_32(_ta4 >> 2);
      if (_ta4 < -1000000) _ta4 = -1000000;
    }
  }
  if (_cycles > 175000) {
    _cycles -= 175000;
    _Z80.IRQ(0x38);
  }
}

void ZxSpectrum::reset() { 
  reset(0x0000); 
  _tu4 = time_us_32() << 2;
  _ta4 = 0;
}

void ZxSpectrum::moderate(bool on) {
  if (on == _moderate) return;
  
  if (on) {
    _tu4 = time_us_32() << 2;
    _ta4 = 0;
  }
  
  _moderate = on;
}

void ZxSpectrum::toggleModerate() {
  moderate(!_moderate);
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
  return is->read(_RAM + (16*1024), (48*1024)); 
}

void ZxSpectrum::loadZ80(InputStream *is) {
  int version = loadZ80Header(is);
  printf("Reading Z80 version %d\n", version);
  if (version < 0) return; // error
  switch(version) {
    case 0:
      loadZ80MemV0(is);
      break;
    default:
      printf("Reading Z80 version %d is not implemented!\n", version);
      break;
  }
}
