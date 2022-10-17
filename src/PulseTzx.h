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
  void indexBlocks();
  void indexBlock(int32_t bt);
  
  void skipStandardSpeedData();
  void skipTurboSpeedData();
  void skipPureTone();
  void skipSequence();
  void skipPureData();
  void skipDirectRecording();
  void skipCswRecording();
  void skipGeneralizedData();
  void skipPause();
  void skipGroupStart();
  void skipGroupEnd();
  void skipJump();
  void skipLoopStart();
  void skipLoopEnd();
  void skipCallSequence();
  void skipReturnFromSequence();
  void skipSelectBlock();
  void skipStopTheTape48k();
  void skipSetSignalLevel();
  void skipTextDescription();
  void skipMessage();
  void skipArchiveInfo();
  void skipHardwareType();
  void skipCustomInfo();
  void skipGlue();
  
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
