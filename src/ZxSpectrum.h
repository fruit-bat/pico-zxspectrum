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
#include "hardware/pwm.h"
#include "ZxSpectrumAudio.h"

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
  uint32_t _buzzer;

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
          return kb ^ (_ear ? (1<<6) : 0) ;
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

  inline void stepBuzzer() {
    uint32_t d = (_port254 >> 4) & 1;
    if (d == 0 && _buzzer > 0) --_buzzer;
    else if (d == 1 && _buzzer < 10) ++_buzzer;
  }
  
  inline int32_t getBuzzerSmoothed() {
    const int32_t a1 = __mul_instruction(_buzzer, 21);
    const int32_t a2 = _ear ? 31 : 0;
    return a1 + a2;
  }
  
  inline uint32_t getBuzzer() {
    return ((_port254 >> 4) ^ _ear) & 1;
  }  
  
public:
  ZxSpectrum(
    ZxSpectrumKeyboard *keyboard1,
    ZxSpectrumKeyboard *keyboard2,
    ZxSpectrumJoystick *joystick
  );
  inline uint8_t* screenPtr() { return (unsigned char*)&_RAM[(_portMem & 8) ? 7 : 5]; }
  void reset(ZxSpectrumType type);
  ZxSpectrumType type() { return _type; }
    
  void __not_in_flash_func(step)()
  {
      int c;
      if (_mute) {
        c = _Z80.step();
        c += _Z80.step();
        c += _Z80.step();
      }
      else {
        uint32_t vA, vB, vC;
        _ay.vol(vA, vB, vC);
        c = _Z80.step();
        stepBuzzer();
        zxSpectrumAudioHandler(vA, vB, vC, getBuzzerSmoothed(), getBuzzer());
        c += _Z80.step();
        stepBuzzer();
        zxSpectrumAudioHandler(vA, vB, vC, getBuzzerSmoothed(), getBuzzer());
        c += _Z80.step();
        stepBuzzer();
        zxSpectrumAudioHandler(vA, vB, vC, getBuzzerSmoothed(), getBuzzer());        
      }
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
      if (tud) _ay.step(tud);
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

  void setEar(bool ear) { _ear = ear; }
  bool getEar() { return _ear; }
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
