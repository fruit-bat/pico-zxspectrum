#pragma once
#pragma GCC optimize ("O2")

#include "Z80.h"
#include "ZxSpectrumKeyboard.h"
#include "InputStream.h"

class ZxSpectrum {
private:
  Z80 _Z80;
  int _cycles;
  uint32_t _tu4;
  int32_t _ta4;
  bool _moderate;
  ZxSpectrumKeyboard *_keyboard;
  int32_t _borderColour;
  
  static inline int readByte(void * context, int address)
  {
    return ((ZxSpectrum*)context)->_RAM[address];
  }

  static inline void writeByte(void * context, int address, int value)
  {
    auto s = ((ZxSpectrum*)context);
    
    if (address < 0x4000) return;
    s->_RAM[address] = value;
  }
  
  static inline int readIO(void * context, int address)
  {
    // printf("readIO %04X\n", address);
    const auto m = (ZxSpectrum*)context;
    switch(address & 0xFF) {
      case 0xFE: return m->_keyboard->read(address);
      default: return 0xff;
    }
  }

  static inline void writeIO(void * context, int address, int value)
  {
    // printf("writeIO %04X %02X\n", address, value);
    const auto m = (ZxSpectrum*)context;
    switch(address & 0xFF) {
      case 0xFE: 
        m->_borderColour = value & 7;
        break;
      default: 
        break;
    }  
  }
  
  static inline int readWord(void * context, int addr) 
  { 
    return readByte(context, addr) | (readByte(context, addr + 1) << 8); 
  }
  
  static inline void writeWord(void * context, int addr, int value) 
  { 
    writeByte(context, addr, value & 0xFF); writeByte(context, addr + 1, value >> 8); 
  }

  uint8_t _RAM[1<<16];

  int loadZ80MemV0(InputStream *inputStream);
  int loadZ80Header(InputStream *inputStream);
  
public:
  ZxSpectrum(
    ZxSpectrumKeyboard *keyboard
  );
  inline unsigned char* screenPtr() { return &_RAM[0x4000]; }
  void reset(unsigned int address);
  void reset();
  void step();
  void moderate(bool on);
  void toggleModerate();
  unsigned int borderColour() { return _borderColour; }
  

  void loadZ80(InputStream *inputStream);
};
