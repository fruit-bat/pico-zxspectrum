#pragma once
#pragma GCC optimize ("O2")

#include "Z80.h"
#include "ZxSpectrumKeyboard.h"
#include "InputStream.h"
#include "OutputStream.h"

class ZxSpectrum {
private:
  Z80 _Z80;
  int _cycles;
  uint32_t _tu4;
  int32_t _ta4;
  bool _moderate;
  ZxSpectrumKeyboard *_keyboard;
  int32_t _borderColour;
  int _port254;
  unsigned char* _pageaddr[4];
  
  inline void setPageaddr(int page, unsigned char *ptr) {
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
    // printf("readIO %04X\n", address);
    const auto m = (ZxSpectrum*)context;
    switch(address & 0xFF) {
      case 0xFE: return m->_keyboard->read(address);
      case 0x1f: return 0; // Kempstone joystick
      default: return 0xff;
    }
  }

  static inline void writeIO(void * context, int address, int value)
  {
    // printf("writeIO %04X %02X\n", address, value);
    const auto m = (ZxSpectrum*)context;
    switch(address & 0xFF) {
      case 0xFE:
        m->_port254 = value;
        m->_borderColour = value & 7;
        break;
      default: 
        break;
    }  
  }

  uint8_t _RAM[8][1<<14];

  int loadZ80MemV0(InputStream *inputStream);
  int loadZ80MemV1(InputStream *inputStream);
  int loadZ80Header(InputStream *inputStream);
  int writeZ80Header(OutputStream *os, int version);
  int writeZ80(OutputStream *os, int version);
  int writeZ80MemV0(OutputStream *os);
public:
  ZxSpectrum(
    ZxSpectrumKeyboard *keyboard
  );
  inline unsigned char* screenPtr() { return (unsigned char*)&_RAM[5]; }
  void reset(unsigned int address);
  void reset();
  void step();
  void interrupt();
  void moderate(bool on);
  void toggleModerate();
  unsigned int borderColour() { return _borderColour; }
  

  void loadZ80(InputStream *inputStream);
};
