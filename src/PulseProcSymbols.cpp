#include "PulseProcSymbols.h"

int32_t PulseProcSymbols::read(
  InputStream* is,
  uint32_t np,
  uint32_t as
) {
  _np = np;
  _as = as;
  _sd.clear();
  
  int32_t r;
  
  for (uint32_t i = 0; i < as; ++i) {
    
    r = is->readByte(); 
    
    if (r < 0) {
      DBG_PULSE("PulseProcSymbols: Error (%ld) reading symbol type\n", r);      
      return PP_ERROR;
    }
    DBG_PULSE("PulseProcSymbols: symbol type %ld\n", r);      

    // read the type byte
    _sd.push_back(static_cast<uint16_t>(r));
    
    // read the symbol data
    for (uint32_t j = 0; j < np; ++j) {
      r = is->readWord(); 
      if (r < 0) {
        DBG_PULSE("PulseProcSymbols: Error (%ld) reading symbol pulse\n", r);      
        return PP_ERROR;
      }
      DBG_PULSE("PulseProcSymbols: symbol pulse %ld\n", r);      

      // read the pulse width byte
      _sd.push_back(static_cast<uint16_t>(r));
    }
  }
  
  return PP_CONTINUE;
}
