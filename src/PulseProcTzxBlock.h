#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include <vector>
#include "PulseProcTap.h"
#include "PulseProcTzxTurbo.h"

class PulseProcTzxBlock : public PulseProc {
private:
  std::vector<uint32_t>* _bi;
  uint32_t _i;
  PulseProcTap* _ppTap;
  PulseProcTzxTurbo _ppTzxTurbo;
  uint32_t _tsPerMs;

  int32_t doBlock(InputStream *is, int32_t bt, PulseProc **top);
  int32_t skipSingle(InputStream *is, const int8_t* l, uint32_t n, uint32_t m);
  int32_t skipOnly(InputStream *is, uint32_t n);
    
  int32_t doStandardSpeedData(InputStream *is, PulseProc **top);
  int32_t doTurboSpeedData(InputStream *is, PulseProc **top);
  int32_t doPureTone(InputStream *is);
  int32_t doSequence(InputStream *is);
  int32_t doPureData(InputStream *is);
  int32_t doDirectRecording(InputStream *is);
  int32_t doCswRecording(InputStream *is);
  int32_t doGeneralizedData(InputStream *is);
  int32_t doPause(InputStream *is);
  int32_t doGroupStart(InputStream *is);
  int32_t doGroupEnd(InputStream *is);
  int32_t doJump(InputStream *is);
  int32_t doLoopStart(InputStream *is);
  int32_t doLoopEnd(InputStream *is);
  int32_t doCallSequence(InputStream *is);
  int32_t doReturnFromSequence(InputStream *is);
  int32_t doSelectBlock(InputStream *is);
  int32_t doStopTheTape48k(InputStream *is);
  int32_t doSetSignalLevel(InputStream *is);
  int32_t doTextDescription(InputStream *is);
  int32_t doMessage(InputStream *is);
  int32_t doArchiveInfo(InputStream *is);
  int32_t doHardwareType(InputStream *is);
  int32_t doCustomInfo(InputStream *is);
  int32_t doGlue(InputStream *is);

public:

  PulseProcTzxBlock(
    PulseProcTap* ppTap,
    PulseProcStdHeader* header,
    PulseProcStdByteStream* data,
    PulseProcTone* end,
    PulseProcPauseMillis* pause
  );
  
  void init(
    PulseProc *next,
    std::vector<uint32_t>* bi,
    uint32_t tsPerMs
  );
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
};
