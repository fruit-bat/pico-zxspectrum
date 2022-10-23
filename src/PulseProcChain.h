#pragma once

#include "PulseProc.h"
#include "InputStream.h"

#include "PulseProcTone.h"
#include "PulseProcTap.h"
#include "PulseProcStdHeader.h"
#include "PulseProcStdByteStream.h"
#include "PulseProcStdByte.h"

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
  PulseProcTap _ppTap;
  
public:

  PulseProcChain();
  
  void init(InputStream *is, PulseProc* top);
  
  void __not_in_flash_func(advance)(uint32_t tstates, bool *pstate);
  
  inline bool end() { return _state < 0; }
  
  void reset();
  void loadTap(InputStream *is);
  void loadTzx(InputStream *is);
  
};
