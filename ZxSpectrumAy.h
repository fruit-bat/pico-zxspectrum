#pragma once

#include <memory.h>
#include <pico/printf.h>

// A 440Hz 2034 ? (or is this 220Hz)
// (1/(2*2034*440)) This is the duration of a tick on the ay in seconds
// (1000000/(2*2034*440)) This is the duration of a tick on the ay in micro seconds

// ((32*1000000)/(2*2034*440)) = 17.8778939841

// ((2*2034*440*(1<<12))/(32*1000000))
// #define STEP 229
#define STEP 229

static const unsigned char Envelopes[16][32] =
{
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 },
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
  { 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 },
  { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }
};

static const int Volumes[16] =
{ 0,1,2,4,6,8,11,16,23,32,45,64,90,128,180,255 };
//{ 0,16,33,50,67,84,101,118,135,152,169,186,203,220,237,254 };

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
  uint32_t _pmA;
  uint32_t _pmB;
  uint32_t _pmC;

  inline uint32_t mixer() { return _reg.r8[7]; }
  inline uint32_t periodA() { return _reg.r16[0]; }
  inline uint32_t periodB() { return _reg.r16[1]; }
  inline uint32_t periodC() { return _reg.r16[2]; }
  inline uint32_t volA() { return Volumes[_reg.r8[8] & 0xf]; }
  inline uint32_t volB() { return Volumes[_reg.r8[9] & 0xf]; }
  inline uint32_t volC() { return Volumes[_reg.r8[10] & 0xf]; }

public:
  ZxSpectrumAy() {
    reset();
  }
  
  void __not_in_flash_func(step)(uint32_t u32s) {
    const uint32_t s = __mul_instruction(u32s, STEP);
    if (_pmA) {
      _cntA += s;
      while(_cntA >= _pmA) { _cntA -= _pmA; _tb ^= 1; }
    }
    else {
      _cntA = 0;
    }
    if (_pmB) {
      _cntB += s;
      while(_cntB >= _pmB) { _cntB -= _pmB; _tb ^= 2; }
    }
    else {
      _cntB = 0;
    }
    if (_pmC) {
      _cntC += s;
      while(_cntC >= _pmC) { _cntC -= _pmC; _tb ^= 4; }
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
    _pmA = 0;
    _pmB = 0;
    _pmC = 0;
  }

  inline void writeCtrl(uint8_t v) {
    _l = v & 0xf;
  }

  inline void writeData(uint8_t v) {
    _reg.r8[_l] = v;
    switch (_l) {
      case 0: case 1:
        _pmA = periodA() << 15;
        break;
      case 2: case 3:
        _pmB = periodB() << 15;
        break;
      case 4: case 5:
        _pmC = periodC() << 15;
        break;       
      default: break;
    }
  }

  inline uint8_t readData() {
    return _reg.r8[_l];
  }

  uint32_t __not_in_flash_func(vol)() {
    uint32_t v = 0;
    const uint32_t m = mixer();
    const uint32_t nm = ~m;

    //if (nm & 1) { const uint32_t vA = volA(); v += (_tb & 1) ? vA: -vA; }
    //if (nm & 2) { const uint32_t vB = volB(); v += (_tb & 2) ? vB: -vB; }
    //if (nm & 4) { const uint32_t vC = volC(); v += (_tb & 4) ? vC: -vC; }

    const int32_t vA = __mul_instruction(volA(), (nm     ) & 1); v += vA - __mul_instruction((_tb << 1) & 2, vA);
    const int32_t vB = __mul_instruction(volB(), (nm >> 1) & 1); v += vB - __mul_instruction((_tb     ) & 2, vB);
    const int32_t vC = __mul_instruction(volC(), (nm >> 2) & 1); v += vC - __mul_instruction((_tb >> 1) & 2, vC);

    
    return v;
  }
};
