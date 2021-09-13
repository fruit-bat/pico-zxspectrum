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
