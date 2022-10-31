#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include <vector>
#include "PulseProcTap.h"
#include "PulseProcTzxTurbo.h"
#include "PulseProcTzxPureTone.h"
#include "PulseProcPulseStream.h"
#include "PulseProcTzxPulseSequence.h"
#include "PulseProcTzxPureData.h"
#include "PulseProcBitStream.h"
#include "PulseProcTzxDirectRecording.h"
#include "PulseProcCallStream.h"

class PulseProcTzxBlock : public PulseProc {
private:
  std::vector<uint32_t>* _bi;
  uint32_t _i;
  uint32_t _loopStart;
  uint32_t _loopCount;
  PulseProcTap* _ppTap;
  PulseProcTzxTurbo _ppTzxTurbo;
  PulseProcTzxPureTone _ppTzxPureTone;
  PulseProcPulseStream _ppPulseStream;
  PulseProcTzxPulseSequence _ppTzxPulseSequence;
  PulseProcTzxPureData _ppTzxPureData;
  PulseProcBitStream _ppBitStream;
  PulseProcTzxDirectRecording _ppTzxDirectRecording;
  PulseProcCallStream _ppCallStream;
  uint32_t _tsPerMs;

  int32_t doBlock(InputStream *is, int32_t bt, PulseProc **top);
  int32_t skipSingle(InputStream *is, const int8_t* l, uint32_t n, uint32_t m);
  int32_t skipOnly(InputStream *is, uint32_t n);
    
  int32_t doStandardSpeedData(InputStream *is, PulseProc **top);
  int32_t doTurboSpeedData(InputStream *is, PulseProc **top);
  int32_t doPureTone(InputStream *is, PulseProc **top);
  int32_t doSequence(InputStream *is, PulseProc **top);
  int32_t doPureData(InputStream *is, PulseProc **top);
  int32_t doDirectRecording(InputStream *is, PulseProc **top);
  int32_t doCswRecording(InputStream *is, PulseProc **top);
  int32_t doGeneralizedData(InputStream *is, PulseProc **top);
  int32_t doPause(InputStream *is, PulseProc **top);
  int32_t doGroupStart(InputStream *is, PulseProc **top);
  int32_t doGroupEnd(InputStream *is, PulseProc **top);
  int32_t doJump(InputStream *is, PulseProc **top);
  int32_t doLoopStart(InputStream *is, PulseProc **top);
  int32_t doLoopEnd(InputStream *is, PulseProc **top);
  int32_t doCallSequence(InputStream *is, PulseProc **top);
  int32_t doReturnFromSequence(InputStream *is, PulseProc **top);
  int32_t doSelectBlock(InputStream *is, PulseProc **top);
  int32_t doStopTheTape48k(InputStream *is, PulseProc **top);
  int32_t doSetSignalLevel(InputStream *is, PulseProc **top);
  int32_t doTextDescription(InputStream *is, PulseProc **top);
  int32_t doMessage(InputStream *is, PulseProc **top);
  int32_t doArchiveInfo(InputStream *is, PulseProc **top);
  int32_t doHardwareType(InputStream *is, PulseProc **top);
  int32_t doCustomInfo(InputStream *is, PulseProc **top);
  int32_t doGlue(InputStream *is, PulseProc **top);

public:

  PulseProcTzxBlock(
    PulseProcTap* ppTap,
    PulseProcStdHeader* header,
    PulseProcStdByteStream* data,
    PulseProcTone* ppTone1,
    PulseProcTone* ppTone2,
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
