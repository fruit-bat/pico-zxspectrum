#pragma once

#include "PulseByte.h"
#include "PulseByteStream.h"
#include "PulsePreamble.h"
#include <cstring>
#include <vector>

class PulseTzx {
private:

  InputStream* _is;
  std::vector<uint32_t> _bi;
  
  int _tstates;
  int32_t indexBlocks();
  int32_t indexBlock(int32_t bt);
  int32_t skipSingle(const int8_t* l, uint32_t n, uint32_t m);
    
  int32_t skipStandardSpeedData();
  int32_t skipTurboSpeedData();
  int32_t skipPureTone();
  int32_t skipSequence();
  int32_t skipPureData();
  int32_t skipDirectRecording();
  int32_t skipCswRecording();
  int32_t skipGeneralizedData();
  int32_t skipPause();
  int32_t skipGroupStart();
  int32_t skipGroupEnd();
  int32_t skipJump();
  int32_t skipLoopStart();
  int32_t skipLoopEnd();
  int32_t skipCallSequence();
  int32_t skipReturnFromSequence();
  int32_t skipSelectBlock();
  int32_t skipStopTheTape48k();
  int32_t skipSetSignalLevel();
  int32_t skipTextDescription();
  int32_t skipMessage();
  int32_t skipArchiveInfo();
  int32_t skipHardwareType();
  int32_t skipCustomInfo();
  int32_t skipGlue();
  
public:

  PulseTzx();
  
  bool __not_in_flash_func(end)();
  
  void __not_in_flash_func(reset)(InputStream* is);
  

  int __not_in_flash_func(advance)(int tstates, bool *pstate) {
    if (end()) return -1;
    _tstates += tstates;
 
    _is->close();
    return 0;
  }
};
