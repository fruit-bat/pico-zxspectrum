#pragma once
#pragma GCC optimize ("O2")

#include "Z80.h"
#include "ZxSpectrumKeyboard.h"
#include "ZxSpectrumJoystick.h"
#include "InputStream.h"
#include "OutputStream.h"
#include <pico/stdlib.h>
#include "PulseBlock.h"
#include "128k_rom_1.h"
#include "128k_rom_2.h"
#include "ZxSpectrumAy.h"

enum ZxSpectrumType {
  ZxSpectrum48k,
  ZxSpectrum128k
};

class ZxSpectrum {
private:
  Z80 _Z80;
  uint32_t _tu32;
  int32_t _ta32;
  uint32_t _moderate;
  ZxSpectrumKeyboard *_keyboard1;
  ZxSpectrumKeyboard *_keyboard2;
  ZxSpectrumJoystick *_joystick;
  uint8_t _borderColour;
  uint8_t _port254;
  uint8_t _portMem;
  uint8_t* _pageaddr[4];
  bool _ear;
	PulseBlock _pulseBlock;
  ZxSpectrumAy _ay;
  ZxSpectrumType _type;
  uint32_t _modMul;
  bool _mute;
  bool _pauseTape;

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
    if (address == 0xfffd) {
      return _ay.readData();
    }
    else {
      switch(address & 0xFF) {
        case 0xFE: {
          uint8_t kb = _keyboard1->read(address);
          if (_keyboard2) kb &= _keyboard2->read(address);
          return kb | (_ear ? (1<<6) : 0) ;
        }
        case 0x1f: return _joystick ? _joystick->getKempston() : 0;
        default: return 0xff;
      }
    }
  }
  
  inline void writeIO(int address, int value) {
    if (address == 0x7ffd) {
      if ((_portMem & 0x20) == 0) { 
        _portMem = value;
        setPageaddr(3, (uint8_t*)&_RAM[value & 7]);
        setPageaddr(0, (uint8_t*)((value & 0x10) ? zx_128k_rom_2 : zx_128k_rom_1));
      }
    }
    else if (address == 0xfffd) {
      _ay.writeCtrl(value);
    }
    else if (address == 0xbffd) {
      _ay.writeData(value);
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

  uint8_t _RAM[8][1<<14];
  
  void transmute(ZxSpectrumType type);
  int loadZ80MemV0(InputStream *is);
  int loadZ80MemV1(InputStream *is);
  int loadZ80Header(InputStream *is);
  int loadZ80HeaderV2(InputStream *is, ZxSpectrumType *type);
  int loadZ80MemBlock(InputStream *is, const ZxSpectrumType type);
  int writeZ80Header(OutputStream *os, int version);
  int writeZ80HeaderV3(OutputStream *os);
  int writeZ80(OutputStream *os, int version);
  int writeZ80MemV0(OutputStream *os);
  int writeZ80MemV1(OutputStream *os);
  int writeZ80MemV2(OutputStream *os, uint8_t *blk);
  int writeZ80MemV2(OutputStream *os, const uint8_t b, const uint8_t p);
  int writeZ80MemV2(OutputStream *os);
  int z80BlockToPage(const uint8_t b, const ZxSpectrumType type);

public:
  ZxSpectrum(
    ZxSpectrumKeyboard *keyboard1,
    ZxSpectrumKeyboard *keyboard2,
    ZxSpectrumJoystick *joystick
  );
  inline uint8_t* screenPtr() { return (unsigned char*)&_RAM[(_portMem & 8) ? 7 : 5]; }
  void reset(ZxSpectrumType type);
  ZxSpectrumType type() { return _type; }
  inline void step()
  {
      const int c = _Z80.step() + _Z80.step() + _Z80.step() + _Z80.step();
      const uint32_t tu32 = time_us_32() << 5;
      const uint32_t tud = tu32 - _tu32;
      _tu32 = tu32;
      if (_moderate) {
        if (c == 0) {
          _ta32 = 0;
        }
        else {
          _ta32 += MUL32(c, _moderate) - tud; // +ve too fast, -ve too slow
          if (_ta32 > 32) busy_wait_us_32(_ta32 >> 5);
          // Try to catch up, but only for 500 or so instructions
          if (_ta32 < -500 * 4 * 32)  _ta32 = -500 * 4 * 32;
        }
      }
      _ay.step(tud);
      _pulseBlock.advance(_pauseTape ? 0 : c, &_ear);
  }

  void interrupt();
  void moderate(uint32_t mul);
  void toggleModerate();
  uint32_t moderate() { return _moderate; }
  void mute(bool mute) { _mute = mute; }
  void toggleMute() { _mute = !_mute; }
  bool mute() { return _mute; }

  inline unsigned int borderColour() { return _borderColour; }
  inline int32_t getSpeaker() {
    if (_mute) return 0;
    const int32_t a1 = (_port254 & (1<<4)) ? 128 : -127;
    const int32_t a2 = _ear ? 64 : -63;
    return a1 + a2 + _ay.vol();
  }
  inline int32_t getAnalogueAudio() {
    return _mute ? 0 : _ay.vol();
  }
  inline bool getBuzzer() {
    return _mute ? false : ((_port254 >> 4) & 1) ^ _ear;
  }
  inline void vol(uint32_t& vA, uint32_t& vB, uint32_t& vC) {
    if (_mute) {
      vA = 0;
      vB = 0;
      vC = 0;
    } 
    else {
      _ay.vol(vA, vB, vC);
    }
  }
  
  void setEar(bool ear) { _ear = ear; }
  void loadZ80(InputStream *inputStream);
  void saveZ80(OutputStream *outputStream);
  void loadTap(InputStream *inputStream);
  bool tapePaused() { return _pauseTape; }
  void pauseTape(bool pause) { _pauseTape = pause; }
  void togglePauseTape() { _pauseTape = !_pauseTape; }
  ZxSpectrumJoystick *joystick() { return _joystick; }
  ZxSpectrumKeyboard *keyboard1() { return _keyboard1; }
  ZxSpectrumKeyboard *keyboard2() { return _keyboard2; }

};
