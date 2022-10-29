#include "PulseProcTzxPulseSequence.h"


PulseProcTzxPulseSequence::PulseProcTzxPulseSequence(
    PulseProcPulseStream* ppPulseStream
) : 
  _ppPulseStream(ppPulseStream)
{
}

void PulseProcTzxPulseSequence::init(
  PulseProc *nxt
) {
  next(nxt);
}

/**
 * ID 13 - Pulse sequence
 * 
 * length: [00]*02+01
 * 
 * Offset	Value	Type	Description
 * 0x00	N	BYTE	Number of pulses
 * 0x01	-	WORD[N]	Pulses' lengths
*/
int32_t __not_in_flash_func(PulseProcTzxPulseSequence::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzxPulseSequence: \n");
  
  const int8_t l[] = {
    1  // 0. BYTE	Number of pulses
  };
  
  uint32_t h[1];
  int32_t r = is->decodeLsbf(h, l, 1);
  
  if (r < 0) {
    DBG_PULSE("PulseProcTzxPulseSequence: Error (%ld) reading pulse sequence header\n", r);
    return PP_ERROR;
  }
  else {
    
    DBG_PULSE("PulseProcTzxPulseSequence: Number of pulses %ld\n", h[0]);

    _ppPulseStream->init(
      next(),
      h[0]  // 0. BYTE	Number of pulses
    );

    *top = _ppPulseStream;
    return PP_CONTINUE;
  }
}
