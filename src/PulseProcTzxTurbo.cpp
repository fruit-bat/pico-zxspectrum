#include "PulseProcTzxTurbo.h"


PulseProcTzxTurbo::PulseProcTzxTurbo(
  PulseProcStdHeader* header,
  PulseProcStdByteStream* data,
  PulseProcTone* end,
  PulseProcPauseMillis* pause
) : 
  _header(header),
  _data(data),
  _end(end),
  _pause(pause)
{
}

void PulseProcTzxTurbo::init(
  PulseProc *nxt,
  uint32_t tsPerMs
) {
  next(nxt);
  _tsPerMs = tsPerMs;
}

/**
 * ID 11 - Turbo Speed Data Block
 * 
 * length: [0F,10,11]+12
 * 
 * Offset	Value	Type	Description
 * ==============================
 * 0x00	-	WORD	Length of PILOT pulse {2168}
 * 0x02	-	WORD	Length of SYNC first pulse {667}
 * 0x04	-	WORD	Length of SYNC second pulse {735}
 * 0x06	-	WORD	Length of ZERO bit pulse {855}
 * 0x08	-	WORD	Length of ONE bit pulse {1710}
 * 0x0A	-	WORD	Length of PILOT tone (number of pulses) {8063 header (flag<128), 3223 data (flag>=128)}
 * 0x0C	-	BYTE	Used bits in the last byte (other bits should be 0) {8}
 * (e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
 * 0x0D	-	WORD	Pause after this block (ms.) {1000}
 * 0x0F	N	BYTE[3]	Length of data that follow
 * 0x12	-	BYTE[N]	Data as in .TAP files
*/
int32_t __not_in_flash_func(PulseProcTzxTurbo::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzxTurbo: \n");
  
  const int8_t l[] = {
    2, // 0. WORD	Length of PILOT pulse {2168}
    2, // 1. WORD	Length of SYNC first pulse {667}
    2, // 2. WORD	Length of SYNC second pulse {735}
    2, // 3. WORD	Length of ZERO bit pulse {855}
    2, // 4. WORD	Length of ONE bit pulse {1710}
    2, // 5. WORD	Length of PILOT tone (number of pulses) {8063 header (flag<128), 3223 data (flag>=128)}
    1, // 6. BYTE	Used bits in the last byte (other bits should be 0) {8}
    2, // 7. WORD	Pause after this block (ms.) {1000}
    3  // 8. BYTE[3]	Length of data that follow
  };
  
  uint32_t h[9];
  int32_t r = is->decodeLsbf(h, l, 9);
  
  if (r < 0) {
    DBG_PULSE("PulseProcTzxTurbo: Error (%ld) reading turbo header\n", r);
    return PP_ERROR;
  }
  else {
    
    DBG_PULSE("PulseProcTzxTurbo: Length of PILOT pulse %ld\n", h[0]);
    DBG_PULSE("PulseProcTzxTurbo: Length of SYNC first pulse %ld\n", h[1]);
    DBG_PULSE("PulseProcTzxTurbo: Length of SYNC second pulse %ld\n", h[2]);
    DBG_PULSE("PulseProcTzxTurbo: Length of ZERO bit pulse %ld\n", h[3]);
    DBG_PULSE("PulseProcTzxTurbo: Length of ONE bit pulse %ld\n", h[4]);
    DBG_PULSE("PulseProcTzxTurbo: Length of PILOT tone (number of pulses) { %ld\n", h[5]);
    DBG_PULSE("PulseProcTzxTurbo: Used bits in the last byte %ld\n", h[6]);
    DBG_PULSE("PulseProcTzxTurbo: Pause after this block %ld\n", h[7]);
    DBG_PULSE("PulseProcTzxTurbo: Length of data that follow %ld\n", h[8]);

    _header->init(
      _data,
      h[5],  // 5. WORD	Length of PILOT tone (number of pulses) 
      h[0],  // 0. WORD	Length of PILOT pulse
      h[1],  // 1. WORD	Length of SYNC first pulse
      h[2]   // 2. WORD	Length of SYNC second pulse
    );

    _data->init(
      _end,
      h[8],  // 8. BYTE[3]	Length of data that follow
      h[3],  // 3. WORD	Length of ZERO bit pulse {855}
      h[4],  // 4. WORD	Length of ONE bit pulse {1710}
      h[6]   // 6. BYTE	Used bits in the last byte
    );

    _end->init(_pause, 0, 1);

    _pause->init(
      next(),
       h[7], // 7. WORD	Pause after this block (ms.) {1000}
       _tsPerMs
    );    

    *top = _header;
    return PP_CONTINUE;
  }
}
