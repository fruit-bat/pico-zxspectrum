#include "PulseProcRleSym.h"

/**
 * PRLE structure format
 * 
 * Offset	Value	Type	Description
 * 0x00	-	BYTE	Symbol to be represented
 * 0x01	-	WORD	Number of repetitions
*/
int32_t __not_in_flash_func(PulseProcRleSym::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  while (_nr == 0) {
    
    if (_ns == 0) return PP_COMPLETE;
    
    const int8_t l[] = {
      1, // 0. BYTE	Symbol to be represented
      2  // 1. 0x01	-	WORD	Number of repetitions
    };
    uint32_t h[2];
    int32_t r = is->decodeLsbf(h, l, 2);
    
    if (r < 0) {
      DBG_PULSE("PulseProcRleSym: Error (%ld) reading prle entry\n", r);
      return PP_ERROR;
    }
    
    _si = h[0];
    _nr = h[1];
    --_ns;

    DBG_PULSE("PulseProcRleSym: Symbols remaining %ld\n", _ns);
    DBG_PULSE("PulseProcRleSym: Symbol to be represented %ld\n", _si);
    DBG_PULSE("PulseProcRleSym: Repetition %ld\n", _nr);
  }
  
  --_nr;
   _symbol->init(this, _si);
  *top = _symbol;
  return PP_CONTINUE;
}
