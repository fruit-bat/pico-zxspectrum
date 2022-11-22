#include "PulseProcTzxPureTone.h"


PulseProcTzxPureTone::PulseProcTzxPureTone(
    PulseProcTone* ppTone
) : 
  _ppTone(ppTone)
{
}

void PulseProcTzxPureTone::init(
  PulseProc *nxt
) {
  next(nxt);
}

/**
 * ID 12 - Pure Tone
 * 
 * length: 04
 * 
 * Offset	Value	Type	Description
 * 0x00	-	WORD	Length of one pulse in T-states
 * 0x02	-	WORD	Number of pulses
*/
int32_t __not_in_flash_func(PulseProcTzxPureTone::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzxPureTone: \n");
  
  const int8_t l[] = {
    2, // 0. WORD	Length of one pulse in T-states
    2  // 1. WORD	Number of pulses
  };
  
  uint32_t h[2];
  int32_t r = is->decodeLsbf(h, l, 2);
  
  if (r < 0) {
    DBG_PULSE("PulseProcTzxPureTone: Error (%ld) reading pure tone header\n", r);
    return PP_ERROR;
  }
  else {
    
    DBG_PULSE("PulseProcTzxPureTone: Length of one pulse in T-states %ld\n", h[0]);
    DBG_PULSE("PulseProcTzxPureTone: Number of pulses %ld\n", h[1]);

    _ppTone->init(
      next(),
      h[0], // 0. WORD	Length of one pulse in T-states
      h[1]  // 1. WORD	Number of pulses
    );

    *top = _ppTone;
    return PP_CONTINUE;
  }
}
