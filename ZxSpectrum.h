#pragma once
#pragma GCC optimize ("O2")

#include "Z80.h"
#include "ZxSpectrumKeyboard.h"
#include "InputStream.h"
#include "OutputStream.h"
#include <pico/stdlib.h>
#include "PulseBlock.h"
#include "128k_rom_1.h"
#include "128k_rom_2.h"

class ZxSpectrum {
private:
  Z80 _Z80;
  uint32_t _tu4;
  int32_t _ta4;
  bool _moderate;
  ZxSpectrumKeyboard *_keyboard;
  uint8_t _borderColour;
  uint8_t _port254;
  uint8_t _portMem;
  uint8_t* _pageaddr[8];
  bool _ear;
	PulseBlock _pulseBlock;

  inline void setPageaddr(int page, uint8_t *ptr) {
    _pageaddr[page] = ptr - (page << 14);
  }
  
  inline unsigned char* memaddr(int address) {
    return address + _pageaddr[address >> 14];
  }
  
  inline int readByte(int address) {
    return *memaddr(address);
  }
  
  inline void writeByte(int address, int value) {
    if (address < 0x4000) return;    
    *(memaddr(address)) = value;
  }
  
  inline int readWord(int addr) { 
    return readByte(addr) | (readByte((addr + 1) & 0xffff) << 8);
  }
  
  inline void writeWord(int addr, int value) { 
    writeByte(addr, value & 0xFF); 
    writeByte((addr + 1) & 0xffff, value >> 8);
  }
  
  inline int readIO(int address)
  {
    switch(address & 0xFF) {
      case 0xFE: return _keyboard->read(address) | (_ear ? (1<<6) : 0) ;
      case 0x1f: return 0; // Kempstone joystick
      default: return 0xff;
    }
  }
  
  inline void writeIO(int address, int value) {
    if (address == 0x7ffd) {

      if ((_portMem & 0x20) == 0) { 
        _portMem = value;
        setPageaddr(7, (unsigned char*)&_RAM[value & 7]);
        setPageaddr(0, (uint8_t*)((value & 0x10) ? zx_128k_rom_2 : zx_128k_rom_1));
      }
    }
    else {
      switch(address & 0xFF) {
        case 0xFE:
          _port254 = value;
          _borderColour = value & 7;
          break;
        default:
          break;
      }
    }
  }
  
  static inline int readByte(void * context, int address) {
    return ((ZxSpectrum*)context)->readByte(address);
  }

  static inline void writeByte(void * context, int address, int value) {
    ((ZxSpectrum*)context)->writeByte(address, value);
  }
  
  // TODO Can addr ever be odd (if not readWord can be simplified)? 
  static inline int readWord(void * context, int addr) { 
    return ((ZxSpectrum*)context)->readWord(addr); 
  }
  
  // TODO Can addr ever be odd (if not writeWord can be simplified)?
  static inline void writeWord(void * context, int addr, int value) { 
    ((ZxSpectrum*)context)->writeWord(addr, value);
  }
  
  static inline int readIO(void * context, int address)
  {
    //printf("readIO %04X\n", address);
    const auto m = (ZxSpectrum*)context;
    return m->readIO(address);
  }

  static inline void writeIO(void * context, int address, int value)
  {
    //printf("writeIO %04X %02X\n", address, value);
    const auto m = (ZxSpectrum*)context;
    m->writeIO(address, value);
  }

  uint8_t _RAM[8][1<<14]; // 8

  int loadZ80MemV0(InputStream *inputStream);
  int loadZ80MemV1(InputStream *inputStream);
  int loadZ80Header(InputStream *inputStream);
  int writeZ80Header(OutputStream *os, int version);
  int writeZ80(OutputStream *os, int version);
  int writeZ80MemV0(OutputStream *os);
  int writeZ80MemV1(OutputStream *os);
public:
  ZxSpectrum(
    ZxSpectrumKeyboard *keyboard
  );
  inline uint8_t* screenPtr() { return (unsigned char*)&_RAM[(_portMem & 8) ? 7 : 5]; }
  void reset(unsigned int address);
  void reset();
  inline void step()
  {
      int c = _Z80.step();
      if (_moderate) {
        uint32_t tu4 = time_us_32() << 5;
        _ta4 += (c*9) - tu4 + _tu4; // +ve too fast, -ve too slow
        _tu4 = tu4;
        if (_ta4 > 32) busy_wait_us_32(_ta4 >> 5);
        // Try to catch up, but only for 100 or so instructions
        if (_ta4 < -100 * 4 * 32)  _ta4 = -100 * 4 * 32;
      }
      _pulseBlock.advance(c, &_ear);
  }
  void interrupt();
  void moderate(bool on);
  void toggleModerate();
  unsigned int borderColour() { return _borderColour; }
  bool getSpeaker() { return (_port254 & (1<<4)) ^ (_ear ? (1<<4) : 0);}
  void setEar(bool ear) { _ear = ear; }
  void loadZ80(InputStream *inputStream);
  void saveZ80(OutputStream *outputStream);
  void loadTap(InputStream *inputStream);
};
