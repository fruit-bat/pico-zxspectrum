#pragma once

#include <string.h>
#include <stdlib.h>
#include <pico/printf.h>

// A 440Hz 2034 ? (or is this 220Hz)
// (1/(2*2034*440)) This is the duration of a tick on the ay in seconds
// (1000000/(2*2034*440)) This is the duration of a tick on the ay in micro seconds

// ((32*1000000)/(2*2034*440)) = 17.8778939841

// ((2*2034*440*(1<<12))/(32*1000000))
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
{ 0,1,2,4,6,8,11,16,23,32,45,64,90,128,180,255 };
//{ 0,16,33,50,67,84,101,118,135,152,169,186,203,220,237,255 };


//static int Lion17_YM_table [32] =
//  { 0, 0, 190, 286, 375, 470, 560, 664,
//    866, 1130, 1515, 1803, 2253, 2848, 3351, 3862,
//    4844, 6058, 7290, 8559, 10474, 12878, 15297, 17787,
//    21500, 26172, 30866, 35676, 42664, 50986, 58842, 65535};
    
class ZxSpectrumAy {

  uint8_t _l;

  uint32_t _cntA;
  uint32_t _cntB;
  uint32_t _cntC;
  uint64_t _cntE;
  uint32_t _cntN;
  int32_t _tb;
  int32_t _tbN;
  uint32_t _indE;
  uint32_t _indN;
  uint32_t _pmA;
  uint32_t _pmB;
  uint32_t _pmC;
  uint64_t _pmE;
  uint32_t _pmN;
  uint32_t _volA;
  uint32_t _volB;
  uint32_t _volC;
  uint8_t *_env;
  uint8_t _envA;
  uint8_t _envB;
  uint8_t _envC;
  int32_t _ns;

  union  {
    uint8_t r8[16];
    uint16_t r16[8];
  } _reg;
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
    reset();
  }
  
  void __not_in_flash_func(step)(uint32_t u32s) {
    const uint32_t s = MUL32(u32s, STEP);
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
    if (_pmE) {
      _cntE += s;
      while(_cntE >= _pmE) {
        _cntE -= _pmE;
        ++_indE;
      }
      while (_indE >= 0x20) _indE -= 0x10;
    }
    else {
      _cntE = 0;
    }
    if (_pmN) {
      _cntN += s;
      while(_cntN >= _pmN) {
        _cntN -= _pmN;
        _ns = (_ns >> 1) | ((((_ns >> 3) ^ _ns) & 1) << 16);
      }
      _tbN = -(_ns & 1);
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
    _tb = 0;
    _tbN = 0;
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
    _ns = 0xffff;
  }

  inline void writeCtrl(uint8_t v) {
    _l = v & 0xf;
  }

  uint8_t readCtrl() {
    return _l;
  }
  
  uint8_t readData(uint8_t r) {
    return _reg.r8[r];
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
        _pmN = periodN() << 17;
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
      case 11: case 12: // 16
        _pmE = ((uint64_t)periodE()) << 18ULL;
        break;
      case 13:
//        if (v != 0xff) {
          _env = (uint8_t *)&_envs[v & 0xf];
          _indE = 0;
//          _cntE = 0;
//        }
        break;
      default:
       break;
    }
  }

  inline uint8_t readData() {
    return _reg.r8[_l];
  }

  inline void vol(uint32_t& vA, uint32_t& vB, uint32_t& vC) {
    const uint32_t m = mixer();
    const uint32_t mtb = (_tb | m) & (_tbN | (m >> 3));   
    const uint8_t ae = _env[_indE];
    vA =  MUL32((mtb >> 0) & 1, _envA ? ae : _volA);
    vB =  MUL32((mtb >> 1) & 1, _envB ? ae : _volB);
    vC =  MUL32((mtb >> 2) & 1, _envC ? ae : _volC);
  }

  int32_t __not_in_flash_func(vol)() {
    uint32_t vA, vB, vC;
    vol(vA, vB, vC);
    return vA + vB + vC;
  }
};
