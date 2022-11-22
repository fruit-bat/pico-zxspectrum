#pragma once

#include <pico/stdlib.h>
#include "PulseProc.h"
#include <vector>
#include <functional>

class PulseProcTzxSelect : public PulseProc {
private:
  std::vector<uint16_t> _offsets;
  std::function<void()> _clearOptions;
  std::function<void(char *)> _addOption;
  std::function<void()> _showOptions;
public:

  PulseProcTzxSelect();
  
  void init(PulseProc *next);
  
  virtual int32_t __not_in_flash_func(advance)(
    InputStream *is,
    bool *pstate,
    PulseProc **top
  );
  
  int32_t offset(uint32_t choice);
  
  void optionHandlers(
    std::function<void()> clearOptions,
    std::function<void(const char *)> addOption,
    std::function<void()> showOptions
  ) {
    _clearOptions = clearOptions; 
    _addOption = addOption;
    _showOptions = showOptions;
  }
};
