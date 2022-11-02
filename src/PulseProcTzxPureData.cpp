#include "PulseProcTzxPureData.h"


PulseProcTzxPureData::PulseProcTzxPureData(
  PulseProcStdByteStream* data,
  PulseProcPauseMillis* pause
) : 
  _data(data),
  _pause(pause)
{
}

void PulseProcTzxPureData::init(
  PulseProc *nxt,
  uint32_t tsPerMs
) {
  next(nxt);
  _tsPerMs = tsPerMs;
}

/**
 * ID 14 - Pure Data Block
 * 
 * length: [07,08,09]+0A
 * 
 * Offset	Value	Type	Description
 * 0x00	-	WORD	Length of ZERO bit pulse
 * 0x02	-	WORD	Length of ONE bit pulse
 * 0x04	-	BYTE	Used bits in last byte (other bits should be 0)
 * (e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
 * 0x05	-	WORD	Pause after this block (ms.)
 * 0x07	N	BYTE[3]	Length of data that follow
 * 0x0A	-	BYTE[N]	Data as in .TAP files
*/
int32_t __not_in_flash_func(PulseProcTzxPureData::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzxPureData: \n");
  
  const int8_t l[] = {
    2, // 0. WORD	Length of ZERO bit pulse {855}
    2, // 1. WORD	Length of ONE bit pulse {1710}
    1, // 2. BYTE	Used bits in the last byte (other bits should be 0) {8}
    2, // 3. WORD	Pause after this block (ms.) {1000}
    3  // 4. BYTE[3]	Length of data that follow
  };
  
  uint32_t h[5];
  int32_t r = is->decodeLsbf(h, l, 5);
  
  if (r < 0) {
    DBG_PULSE("PulseProcTzxPureData: Error (%ld) reading pure data header\n", r);
    return PP_ERROR;
  }
  else {
    
    DBG_PULSE("PulseProcTzxPureData: Length of ZERO bit pulse %ld\n", h[0]);
    DBG_PULSE("PulseProcTzxPureData: Length of ONE bit pulse %ld\n", h[1]);
    DBG_PULSE("PulseProcTzxPureData: Used bits in the last byte %ld\n", h[2]);
    DBG_PULSE("PulseProcTzxPureData: Pause after this block %ld\n", h[3]);
    DBG_PULSE("PulseProcTzxPureData: Length of data that follow %ld\n", h[4]);

    _data->init(
      _pause,
      h[4],  // 4. BYTE[3]	Length of data that follow
      h[0],  // 0. WORD	Length of ZERO bit pulse {855}
      h[1],  // 1. WORD	Length of ONE bit pulse {1710}
      h[2]   // 2. BYTE	Used bits in the last byte
    );

    _pause->init(
      next(),
       h[3], // 3. WORD	Pause after this block (ms.) {1000}
       _tsPerMs
    );    

    *top = _data;
    return PP_CONTINUE;
  }
}
