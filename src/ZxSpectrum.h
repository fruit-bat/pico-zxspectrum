#pragma once
#pragma GCC optimize ("O2")

#include "Z80.h"
#include "ZxSpectrumKeyboard.h"
#include "ZxSpectrumJoystick.h"
#include "InputStream.h"
#include "OutputStream.h"
#include <pico/stdlib.h>
#include "PulseProcChain.h"
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
  uint32_t _earInvert;
  uint32_t _earDc;
  
  PulseProcChain _pulseChain;
  
  ZxSpectrumAy _ay;
  ZxSpectrumType _type;
  uint32_t _modMul;
  bool _mute;
  uint32_t _buzzer;
  
  uint32_t tStatesPerMilliSecond();
  
  inline uint32_t z80Step(uint32_t tstates) {
    return z80_run(&_Z80, tstates);
  }
  
  inline void z80Power(bool state) {
    z80_power(&_Z80, true);
  }
  
  inline void z80Reset() {
    z80_instant_reset(&_Z80);
  }
  
  inline void setPageaddr(int page, uint8_t *ptr) {
    _pageaddr[page] = ptr - (page << 14);
  }
  
  inline unsigned char* memaddr(uint16_t address) {
    return address + _pageaddr[address >> 14];
  }
  
  inline int readByte(uint16_t address) {
    return *memaddr(address);
  }
  
  inline void writeByte(uint16_t address, uint8_t value) {
    if (address < 0x4000) return;    
    *(memaddr(address)) = value;
  }
  
  inline uint8_t readIO(uint16_t address)
  {
    if (!(address & 0x0001)) {
      uint8_t kb = _keyboard1->read(address);
      if (_keyboard2) kb &= _keyboard2->read(address);
      return (kb & 0xbf) | (((_ear ^ _earInvert) << 6) & (1 << 6));
    }
    if (address == 0xfffd) {
      return _ay.readData();
    }
    if (address == 0x7ffd) {
      // reading #7FFD port is the same as writing #FF into it.
      int value = 0xff;
      _portMem = value;
      setPageaddr(3, (uint8_t*)&_RAM[value & 7]);
      setPageaddr(0, (uint8_t*)((value & 0x10) ? zx_128k_rom_2 : zx_128k_rom_1));
      return 0xff;
    }
    else if (!(address & 0x00e0)) {
       return _joystick ? _joystick->getKempston() : 0;
    }
    else {
      return 0xff;
    }
  }
  
  inline void writeIO(uint16_t address, uint8_t value)
  {
    if (!(address & 0x0001)) {
      _port254 = value;
      _borderColour = value & 7;
    }
    else if (address  == 0x7ffd) {
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
  }
  
  static uint8_t __not_in_flash_func(readByte)(void * context, uint16_t address) {
    return ((ZxSpectrum*)context)->readByte(address);
  }
  
  static void __not_in_flash_func(writeByte)(void * context, uint16_t address, uint8_t value) {
    ((ZxSpectrum*)context)->writeByte(address, value);
  }
   
  static uint8_t __not_in_flash_func(readIO)(void * context, uint16_t address) {
    return ((ZxSpectrum*)context)->readIO(address);
  }

  static void __not_in_flash_func(writeIO)(void * context, uint16_t address, uint8_t value) {
    ((ZxSpectrum*)context)->writeIO(address, value);
  }
  
  static uint8_t __not_in_flash_func(readInt)(void * context, uint16_t address) {
    z80_int(&(((ZxSpectrum*)context)->_Z80), false);
    return 0xff;
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
      uint32_t c;
      if (_mute) {
        c = z80Step(32);
      }
      else {
        uint32_t vA, vB, vC;
        _ay.vol(vA, vB, vC);
        c = z80Step(32);
        stepBuzzer();
        zxSpectrumAudioHandler(vA, vB, vC, getBuzzerSmoothed(), getBuzzer());
        c += z80Step(32);
        stepBuzzer();
        zxSpectrumAudioHandler(vA, vB, vC, getBuzzerSmoothed(), getBuzzer());
        c += z80Step(32);
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
      _pulseChain.advance(c, &_ear);
  }

#define EAR_BITS_PER_STEP 32

  void __not_in_flash_func(step)(uint32_t eb)
  {
      uint32_t vA, vB, vC;

      const uint32_t tu32 = time_us_32() << 5;
      int32_t tud = tu32 - _tu32;
      if (tud) _ay.step(tud); 
      _tu32 = tu32;
      _ay.vol(vA, vB, vC);
      
      if (_earInvert ? (eb == 0) : (~eb == 0)) {
        if (_earDc++ > 16000) {
           _earInvert ^= 1;
        }
      }
      else {
        _earDc = 0;
      }
      
      uint32_t c = 0;
      for (int i = 0; i < EAR_BITS_PER_STEP; ++i) {
        _ta32 += 32;
        if (_pulseChain.end()) _ear = (eb >> i) & 1;

        while (_ta32 > 0) {
          uint32_t t = z80Step(12);
          c += t;
          if (!_mute) {
            stepBuzzer();
            zxSpectrumAudioHandler(vA, vB, vC, getBuzzerSmoothed(), getBuzzer());
          }
          if (t == 0) {
            _ta32 = 0;
            break;
          }
          _ta32 -= MUL32(t, _moderate);
        }
        if ((i&3)==3 && _pulseChain.playing()) {
          _pulseChain.advance(c, &_ear);
          c = 0;
        }
      }
  }

  void interrupt() {
    z80_int(&_Z80, true);
  }

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
  void loadTzx(InputStream *inputStream);
  void ejectTape();
  bool tapePaused();
  void pauseTape(bool pause);
  void togglePauseTape();
  ZxSpectrumJoystick *joystick() { return _joystick; }
  ZxSpectrumKeyboard *keyboard1() { return _keyboard1; }
  ZxSpectrumKeyboard *keyboard2() { return _keyboard2; }
  
  void tzxOptionHandlers(
    std::function<void()> clearOptions,
    std::function<void(const char *)> addOption,
    std::function<void()> showOptions
  ) {
    _pulseChain.optionHandlers(
      clearOptions,
      addOption,
      showOptions
    );
  }
  void tzxOption(uint32_t option) { _pulseChain.option(option); }
  
};
