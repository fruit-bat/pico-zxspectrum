#pragma once

#include <memory.h>
#include <stdlib.h>
#include <pico/printf.h>

// A 440Hz 2034 ? (or is this 220Hz)
// (1/(2*2034*440)) This is the duration of a tick on the ay in seconds
// (1000000/(2*2034*440)) This is the duration of a tick on the ay in micro seconds

// ((32*1000000)/(2*2034*440)) = 17.8778939841

// ((2*2034*440*(1<<12))/(32*1000000))
// #define STEP 229
#define STEP 229
#define MUL32 __mul_instruction

static const uint8_t Envelopes[16][32] =
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

static const uint8_t Volumes[16] =
//{ 0,1,2,4,6,8, 11,16,23,32,45,64,90,128,180,255 };
{ 0,16,33,50,67,84,101,118,135,152,169,186,203,220,237,254 };

class ZxSpectrumAy {

  uint8_t _l;

  uint32_t _cntA;
  uint32_t _cntB;
  uint32_t _cntC;
  uint32_t _cntE;
  uint32_t _cntN;
  int32_t _tbA;
  int32_t _tbB;
  int32_t _tbC;
  int32_t _tbN;
  uint32_t _indE;
  uint32_t _indN;
  uint32_t _pmA;
  uint32_t _pmB;
  uint32_t _pmC;
  uint32_t _pmE;
  uint32_t _pmN;
  uint32_t _volA;
  uint32_t _volB;
  uint32_t _volC;
  uint8_t *_env;
  uint8_t _envA;
  uint8_t _envB;
  uint8_t _envC;

  union  {
    uint8_t r8[16];
    uint16_t r16[8];
  } _reg;
  uint32_t _rand[32];
  uint8_t _envs[16][32];

  inline uint32_t mixer() { return _reg.r8[7]; }
  inline uint32_t periodA() { return _reg.r16[0] & 0x0fff; }
  inline uint32_t periodB() { return _reg.r16[1] & 0x0fff; }
  inline uint32_t periodC() { return _reg.r16[2] & 0x0fff; }
  inline uint32_t periodE() { return ((uint32_t)_reg.r8[11]) + (((uint32_t)_reg.r8[12]) << 8); }
  inline uint32_t periodN() { return _reg.r8[6] & 0x1f; }
  inline uint32_t volA() { return Volumes[_reg.r8[8] & 0xf]; }
  inline uint32_t volB() { return Volumes[_reg.r8[9] & 0xf]; }
  inline uint32_t volC() { return Volumes[_reg.r8[10] & 0xf]; }

public:
  ZxSpectrumAy() {
    for (uint32_t i = 0; i < 16; ++ i) {
      for (uint32_t j = 0; j < 32; ++ j) {
        _envs[i][j] = Volumes[Envelopes[i][j]];
      }
    }
    for (uint32_t i = 0; i < (sizeof(_rand)/sizeof(uint32_t)); ++ i) {
      _rand[i] = rand();
    }
    reset();
  }
  
  void __not_in_flash_func(step)(uint32_t u32s) {
    const uint32_t s = MUL32(u32s, STEP);
    if (_pmA) {
      _cntA += s;
      while(_cntA >= _pmA) { _cntA -= _pmA; _tbA = -_tbA; }
    }
    else {
      _cntA = 0;
    }
    if (_pmB) {
      _cntB += s;
      while(_cntB >= _pmB) { _cntB -= _pmB; _tbB = -_tbB; }
    }
    else {
      _cntB = 0;
    }
    if (_pmC) {
      _cntC += s;
      while(_cntC >= _pmC) { _cntC -= _pmC; _tbC = -_tbC; }
    }
    else {
      _cntC = 0;
    }
    if (_pmE) {
      _cntE += s >> 1;
      while(_cntE >= _pmE) {
        _cntE -= _pmE;
        ++_indE;
      }
      if (_indE >= 0x20) _indE = 0x10 | (_indE & 0x0F);
    }
    else {
      _cntE = 0;
    }
    if (_pmN) {
      _cntN += s;
      while(_cntN >= _pmN) {
        _cntN -= _pmN;
        _indN = (_indN + 1) & ((sizeof(_rand)<<3) - 1);
        _tbN = (_rand[_indN >> 5] >> (_indN & 0x1f)) & 1 ? 1: -1;
      }
    }
    else {
      _cntN = 0;
    }
  }

  inline void reset() {
    memset(_reg.r8, 0, sizeof(_reg.r8));
    _reg.r8[7] = 0xFD;
    _reg.r8[14] = 0xFF;

    _cntA = 0;
    _cntB = 0;
    _cntC = 0;
    _cntE = 0;
    _cntN = 0;
    _indE = 0;
    _indN = 0;
    _tbA = -1;
    _tbB = -1;
    _tbC = -1;
    _tbN = -1;
    _pmA = 0;
    _pmB = 0;
    _pmC = 0;
    _pmE = 0;
    _pmN = 0;
    _volA = 0;
    _volB = 0;
    _volC = 0;
    _env = (uint8_t *)&_env[0];
    _envA = 0;
    _envB = 0;
    _envC = 0;
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
      case 6:
        _pmN = periodN() << 15;
        break;
      case 8:
        _volA = volA();
        _envA = (v & 0x10);
        break;
      case 9:
        _volB = volB();
        _envB = (v & 0x10);
        break;
      case 10:
        _volC = volC();
        _envC = (v & 0x10);
        break;
      case 11: case 12:
        _pmE = periodE() << 15;
        _indE = 0; // ?
        _cntE = 0; // ?
        break;
      case 13:
        _env = (uint8_t *)&_envs[v & 0xf];
        _indE = 0; // ?
        _cntE = 0; // ?
        break;
      default:
       break;
    }
  }

  inline uint8_t readData() {
    return _reg.r8[_l];
  }

  int32_t __not_in_flash_func(vol)() {
    int32_t v = 0;
    const uint32_t m = mixer();
    const uint32_t nm = ~m;
    const uint8_t ae = _env[_indE];
    const uint32_t aA = _envA ? ae : _volA;
    const uint32_t aB = _envB ? ae : _volB;
    const uint32_t aC = _envC ? ae : _volC;
    
    if (nm & 0x01) {
      v += MUL32(_tbA, aA);
    }
    if (nm & 0x02) {
      v += MUL32(_tbB, aB);
    }
    if (nm & 0x04) {
      v += MUL32(_tbC, aC);
    }
    if (nm & 0x08) {
      v += MUL32(_tbN, aA);
    }
    if (nm & 0x10) {
      v += MUL32(_tbN, aB);
    }
    if (nm & 0x20) {
      v += MUL32(_tbN, aC);
    }
    
    return v;
  }
};
