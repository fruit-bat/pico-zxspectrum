#pragma once

#include <memory.h>
#include <pico/printf.h>

// A 440Hz 2034 ? (or is this 220Hz)
// (1/(2*2034*440)) This is the duration of a tick on the ay in seconds
// (1000000/(2*2034*440)) This is the duration of a tick on the ay in micro seconds

// ((32*1000000)/(2*2034*440)) = 17.8778939841

// ((2*2034*440*(1<<12))/(32*1000000))
#define STEP 229
// Say we step 
static const int Volumes[16] =
{ 0,1,2,4,6,8,11,16,23,32,45,64,90,128,180,255 };

class ZxSpectrumAy {

  union  {
    uint8_t r8[16];
    uint16_t r16[8];
  } _reg;

  uint8_t _l;

  uint32_t _cntA;
  uint32_t _cntB;
  uint32_t _cntC;
  uint32_t _tb;

  inline uint8_t mixer() { return _reg.r8[7]; }
  inline uint16_t periodA() { return _reg.r16[0]; }
  inline uint16_t periodB() { return _reg.r16[1]; }
  inline uint16_t periodC() { return _reg.r16[2]; }
  inline uint8_t volA() { return mixer() & 0x1 ? 0 : Volumes[_reg.r8[8] & 0xf]; }
  inline uint8_t volB() { return mixer() & 0x2 ? 0 : Volumes[_reg.r8[9] & 0xf]; }
  inline uint8_t volC() { return mixer() & 0x4 ? 0 : Volumes[_reg.r8[10] & 0xf]; }

public:
  ZxSpectrumAy() {
    reset();
  }
  
  inline void step(uint32_t u32s) {
    const uint32_t s = u32s * STEP;
    const uint32_t pmA = periodA() << 12;
    if (pmA > 0) {
      _cntA += s;
      while(_cntA >= pmA) { _cntA -= pmA; _tb ^= 1; }
    }
    else {
      _cntA = 0;
    }
    const uint32_t pmB = periodB() << 12;
    if (pmB > 0) {
      _cntB += s;
      while(_cntB >= pmB) { _cntB -= pmB; _tb ^= 2; }
    }
    else {
      _cntB = 0;
    }
    const uint32_t pmC = periodC() << 12;
    if (pmC > 0) {
      _cntC += s;
      while(_cntC >= pmC) { _cntC -= pmC; _tb ^= 4; }
    }
    else {
      _cntC = 0;
    }
  }

  inline void reset() {
    memset(_reg.r8, 0, sizeof(_reg.r8));
    _reg.r8[7] = 0xFD;
    _reg.r8[14] = 0xFF;

    _cntA = 0;
    _cntB = 0;
    _cntC = 0;
    _tb = 0;
  }

  inline void writeCtrl(uint8_t v) {
    _l = v & 0xf;
  }

  inline void writeData(uint8_t v) {
    _reg.r8[_l] = v;
  }

  inline uint8_t readData() {
    return _reg.r8[_l];
  }
};
