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
#include "ZxSpectrumMouse.h"
#include "ZxSpectrumPort.h"

// #define DEBUG_SPEC
#ifdef DEBUG_SPEC
#define DBG_SPEC(...) printf(__VA_ARGS__)
#else
#define DBG_SPEC(...)
#endif

enum ZxSpectrumType {
  ZxSpectrum48k,
  ZxSpectrum128k
};

enum ZxSpectrumIntSource {
  SyncToDisplay = 0,
  SyncToCpu     = 1
};

class ZxSpectrum {
private:
  Z80 _Z80;
  uint32_t _tu32;
  int32_t _ta32; // audio out timer
  int32_t _te32; // audio in timer
  uint32_t _moderate;
  ZxSpectrumKeyboard *_keyboard1;
  ZxSpectrumKeyboard *_keyboard2;
  ZxSpectrumJoystick *_joystick;
  ZxSpectrumMouse *_mouse;
  ZxSpectrumPort *_parallelPort;
  uint8_t _borderColour;
  uint8_t _port254;
  uint8_t _portMem;
  uint8_t* _pageaddr[4];
  bool _ear;
  uint32_t _earInvert;
  uint32_t _earDc;
  uint32_t _eb; // 32 audio in samples
  uint32_t _ebc; // index into the above
  
  PulseProcChain _pulseChain;
  
  ZxSpectrumAy _ay;
  ZxSpectrumType _type;
  ZxSpectrumIntSource _intSource;
  uint32_t _modMul;
  bool _mute;
  uint32_t _buzzer;
  uint32_t _sl;
  uint32_t _slc;
  uint32_t _fc;  // Screen flip count
  uint32_t _fcf; // Screen flip count per frame
  
  
  volatile uint8_t _borderBuf[240];       //Border Buffer 240 lines

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
  
  inline void setPageaddr(uint32_t page, uint8_t *ptr) {
    _pageaddr[page] = ptr - (page << 14);
  }
  
  inline uint8_t* memaddr(uint32_t address) {
    return address + _pageaddr[address >> 14];
  }
  
  inline uint8_t readByte(uint16_t address) {
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
      return (kb & 0xBF) | (((_ear ^ _earInvert) << 6) & (1 << 6));
    }else
    if (address == 0xFFFD) {
      return _ay.readData();
    }else
    if (address == 0x7FFD && _type != ZxSpectrum48k) {
      // reading #7FFD port is the same as writing #FF into it.
      uint8_t value = 0xFF;
      _portMem = value;
      setPageaddr(3, (uint8_t*)&_RAM[value & 7]);
      setPageaddr(0, (uint8_t*)((value & 0x10) ? zx_128k_rom_2 : zx_128k_rom_1));
      return 0xFF;
    }

    if ((!(address==0x7FFD))&&(!(address&0x00E0))) {
       return (_joystick ? _joystick->getKempston() : 0) | (_mouse ? _mouse->getKempston() : 0);
    }

    // Kempston mouse
    // 64479	FBDF	xxxx x011 xx0x xxxx	X axis	00-FF Roll over at 0 and $FF
    // 65503	FFDF	xxxx x111 xx0x xxxx	Y Axis	00-FF Roll over at 0 and $FF
    // 64223	FADF	xxxx x010 xx0x xxxx	Buttons	D0:Right D1:Left D2:Middle (Some compatables only)
    if ((address | 0b1111100011011111) == 0xFBDF) {
      return _mouse->xAxis();
    }
    if ((address | 0b1111100011011111) == 0xFFDF) {
      return _mouse->yAxis();
    }
    if ((address | 0b1111100011011111) == 0xFADF) {
      return _mouse->buttons();
    }

    if (address == 0x0FFF && _parallelPort) {
      return _parallelPort->read();
    }

    return 0xFF;
  }

inline void writeIO(uint16_t address, uint8_t value)
  {
	  if (address & 0x0001)
	  {
      if ((address & 0xC002) == 0xC000)
	  	{ 
		  	_ay.writeCtrl(value);
			  return;
  		} //FFFD
		
      if ((address & 0xC002) == 0x8000) 
		  { 
			  _ay.writeData(value);
  			return;
	  	} //BFFD
		
      if (!(address & 0x8002) && (_type != ZxSpectrum48k))
  		{
        if ((_portMem & 0x20) == 0) { 
          _fc += ((_portMem ^ value) >> 3) & 1;
          _portMem = value;
          setPageaddr(3, (uint8_t*)&_RAM[value & 7]);
          setPageaddr(0, (uint8_t*)((value & 0x10) ? zx_128k_rom_2 : zx_128k_rom_1));
        }
			  return;
		  };

      if (address == 0x0FFF && _parallelPort) {
        _parallelPort->write(value);
      }
  	}
    else
	    {
        _port254 = value;
        _borderColour = value & 7;
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
  int32_t loadZ80MemV0(InputStream *is);
  int32_t loadZ80MemV1(InputStream *is);
  int32_t loadZ80Header(InputStream *is);
  int32_t loadZ80HeaderV2(InputStream *is, ZxSpectrumType *type);
  int32_t loadZ80MemBlock(InputStream *is, const ZxSpectrumType type);
  int32_t writeZ80Header(OutputStream *os, int32_t version);
  int32_t writeZ80HeaderV3(OutputStream *os);
  int32_t writeZ80(OutputStream *os, int32_t version);
  int32_t writeZ80MemV0(OutputStream *os);
  int32_t writeZ80MemV1(OutputStream *os);
  int32_t writeZ80MemV2(OutputStream *os, uint8_t *blk);
  int32_t writeZ80MemV2(OutputStream *os, const uint8_t b, const uint8_t p);
  int32_t writeZ80MemV2(OutputStream *os);
  int32_t z80BlockToPage(const uint8_t b, const ZxSpectrumType type);

  inline void stepBuzzer() {
    uint32_t d = (_port254 >> 4) & 1;
    if (d == 0 && _buzzer > 0) --_buzzer;
    else if (d == 1 && _buzzer < 5) ++_buzzer;
  }
  
  inline int32_t getBuzzerSmoothed() {
    const int32_t a1 = __mul_instruction(_buzzer, 42);
    const int32_t a2 = _ear ? 31 : 0;
    return a1 + a2;
  }
  
  inline uint32_t getBuzzer() {
    return ((_port254 >> 4) ^ _ear) & 1;
  }

  void stepScanline(const uint32_t c);

  inline void interrupt() {
    z80_int(&_Z80, true);
    _Z80.int_line = false;
    _fcf = _fc;
    _fc = 0;
  }

public:
  ZxSpectrum(
    ZxSpectrumKeyboard *keyboard1,
    ZxSpectrumKeyboard *keyboard2,
    ZxSpectrumJoystick *joystick,
    ZxSpectrumMouse *mouse,
    ZxSpectrumPort *parallelPort
  );
  inline uint8_t* screenPtr() { return (unsigned char*)&_RAM[(_portMem & 8) ? 7 : 5]; }
  inline uint8_t* memPtr(uint32_t i) { return (unsigned char*)&_RAM[i]; }
  inline uint32_t flipsPerFrame() { return _fcf; }
  void reset(ZxSpectrumType type);
  ZxSpectrumType type() { return _type; }
  ZxSpectrumIntSource intSource() { return _intSource; }
  void intSource(ZxSpectrumIntSource intSource) { _intSource = intSource; }
    
  uint32_t step();

  inline void vsync() {
    if (_intSource == SyncToDisplay) {
      interrupt();
      _sl = 0; 
      _slc = 0;
    }
  }

  void moderate(uint32_t mul);
  void toggleModerate();
  uint32_t moderate() { return _moderate; }
  void mute(bool mute) { _mute = mute; }
  void toggleMute() { _mute = !_mute; }
  bool mute() { return _mute; }

  inline uint8_t borderColour() { return _borderColour; }
  inline uint8_t borderColour(uint32_t y) { return _borderBuf[y]; }

  void setEar(bool ear) { _ear = ear; }
  bool getEar() { return _ear; }
  void loadZ80(InputStream *inputStream);
  bool saveZ80(OutputStream *outputStream);
  void loadTap(InputStream *inputStream);
  void loadTzx(InputStream *inputStream);
  void ejectTape();
  bool tapePaused();
  void pauseTape(bool pause);
  void togglePauseTape();
  ZxSpectrumJoystick *joystick() { return _joystick; }
  ZxSpectrumKeyboard *keyboard1() { return _keyboard1; }
  ZxSpectrumKeyboard *keyboard2() { return _keyboard2; }
  ZxSpectrumMouse *mouse() { return _mouse; }
  
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

  void toggleIntSource() { 
    switch(_intSource) {
      case SyncToCpu: _intSource = SyncToDisplay; break;
      default: _intSource = SyncToCpu; break;
    }
   }
};
