#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include <vector>

class PulseProcTzxIndex : public PulseProc {
private:
  std::vector<uint32_t>* _bi;
  
  int32_t indexBlock(InputStream *is, int32_t bt);
  int32_t skipSingle(InputStream *is, const int8_t* l, uint32_t n, uint32_t m);
  int32_t skipOnly(InputStream *is, uint32_t n);
    
  int32_t skipStandardSpeedData(InputStream *is);
  int32_t skipTurboSpeedData(InputStream *is);
  int32_t skipPureTone(InputStream *is);
  int32_t skipSequence(InputStream *is);
  int32_t skipPureData(InputStream *is);
  int32_t skipDirectRecording(InputStream *is);
  int32_t skipCswRecording(InputStream *is);
  int32_t skipGeneralizedData(InputStream *is);
  int32_t skipPause(InputStream *is);
  int32_t skipGroupStart(InputStream *is);
  int32_t skipGroupEnd(InputStream *is);
  int32_t skipJump(InputStream *is);
  int32_t skipLoopStart(InputStream *is);
  int32_t skipLoopEnd(InputStream *is);
  int32_t skipCallSequence(InputStream *is);
  int32_t skipReturnFromSequence(InputStream *is);
  int32_t skipSelectBlock(InputStream *is);
  int32_t skipStopTheTape48k(InputStream *is);
  int32_t skipSetSignalLevel(InputStream *is);
  int32_t skipTextDescription(InputStream *is);
  int32_t skipMessage(InputStream *is);
  int32_t skipArchiveInfo(InputStream *is);
  int32_t skipHardwareType(InputStream *is);
  int32_t skipCustomInfo(InputStream *is);
  int32_t skipGlue(InputStream *is);

public:

  PulseProcTzxIndex();
  
  void init(PulseProc *next, std::vector<uint32_t>* bi);
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
