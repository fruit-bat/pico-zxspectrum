#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include <vector>
#include "PulseProcTzxHeader.h"
#include "PulseProcTzxGlue.h"
#include "PulseProcTzxIndex.h"
#include "PulseProcTzxBlock.h"
#include <functional>

class PulseProcTzx : public PulseProc {
private:
  std::vector<uint32_t> _bi;
  PulseProcTzxHeader _pptHeader;
  PulseProcTzxGlue _pptGlue;
  PulseProcTzxIndex _pptIndex;
  PulseProcTzxBlock _pptBlock;
  uint32_t _tsPerMs;
  PulseProcPauseMillis _lastPause;
  bool _is48k;

public:

  PulseProcTzx(
    PulseProcTap* ppTap,
    PulseProcStdHeader* header,
    PulseProcStdByteStream* data,
    PulseProcTone* ppTone1,
    PulseProcPauseMillis* pause
  );
  
  void init(
    PulseProc *next,
    uint32_t tsPerMs,
    bool is48k
  );
  
  virtual int32_t advance(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );

  void optionHandlers(
    std::function<void()> clearOptions,
    std::function<void(const char *)> addOption,
    std::function<void()> showOptions
  ) {
    _pptBlock.optionHandlers(
      clearOptions,
      addOption,
      showOptions
    );
  }
  
  void option(uint32_t option) { _pptBlock.option(option); }
  
};
