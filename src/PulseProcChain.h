#pragma once

#include "PulseProc.h"
#include "InputStream.h"

#include "PulseProcTone.h"
#include "PulseProcTap.h"
#include "PulseProcStdHeader.h"
#include "PulseProcStdByteStream.h"
#include "PulseProcStdByte.h"
#include "PulseProcTzx.h"
#include "PulseProcPauseMillis.h"

class PulseProcChain {
private:
  InputStream *_is;
  PulseProc* _top;
  int32_t _state;
  uint32_t _acc;
  
  PulseProcTone _ppTone1;
  PulseProcTone _ppTone2;
  PulseProcStdByte _ppStdByte;
  PulseProcStdByteStream _ppStdByteStream;
  PulseProcStdHeader _ppStdHeader;
  PulseProcPauseMillis _ppPause;
  PulseProcTap _ppTap;
  PulseProcTzx _ppTzx;
  
public:

  PulseProcChain();
  
  void init(InputStream *is, PulseProc* top);
  
  void __not_in_flash_func(advance)(uint32_t tstates, bool *pstate);
  
  inline bool end() { return _state < 0; }
  
  void reset();
  
  void loadTap(
    InputStream *is,
    uint32_t tsPerMs
  );
    
  void loadTzx(
    InputStream *is,
    uint32_t tsPerMs,
    bool is48k
  );
  
  void pause(bool pause);
  
  inline bool paused() { return _state == PP_PAUSE; }
};
