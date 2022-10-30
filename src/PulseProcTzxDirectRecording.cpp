#include "PulseProcTzxDirectRecording.h"


PulseProcTzxDirectRecording::PulseProcTzxDirectRecording(
  PulseProcBitStream* data,
  PulseProcTone* end,
  PulseProcPauseMillis* pause
) : 
  _data(data),
  _end(end),
  _pause(pause)
{
}

void PulseProcTzxDirectRecording::init(
  PulseProc *nxt,
  uint32_t tsPerMs
) {
  next(nxt);
  _tsPerMs = tsPerMs;
}

/**
 * ID 15 - Direct Recording
 * 
 * length: [05,06,07]+08
 * 
 * Offset	Value	Type	Description
 * 0x00	-	WORD	Number of T-states per sample (bit of data)
 * 0x02	-	WORD	Pause after this block in milliseconds (ms.)
 * 0x04	-	BYTE	Used bits (samples) in last byte of data (1-8)
 * (e.g. if this is 2, only first two samples of the last byte will be played)
 * 0x05	N	BYTE[3]	Length of samples' data
 * 0x08	-	BYTE[N]	Samples data. Each bit represents a state on the EAR port (i.e. one sample).
 * MSb is played first.
*/
int32_t __not_in_flash_func(PulseProcTzxDirectRecording::advance)(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzxDirectRecording: \n");
  
  const int8_t l[] = { //TODO
    2, // 0. WORD	Number of T-states per sample (bit of data)
    2, // 1. WORD	Pause after this block in milliseconds (ms.)
    1, // 2. BYTE	Used bits (samples) in last byte of data (1-8)
    3  // 3. BYTE[3]	Length of samples' data
  };
  
  uint32_t h[5];
  int32_t r = is->decodeLsbf(h, l, 5);
  
  if (r < 0) {
    DBG_PULSE("PulseProcTzxDirectRecording: Error (%ld) reading pure data header\n", r);
    return PP_ERROR;
  }
  else {
    
    DBG_PULSE("PulseProcTzxDirectRecording: Number of T-states per sample (bit of data) %ld\n", h[0]);
    DBG_PULSE("PulseProcTzxDirectRecording: Pause after this block in milliseconds (ms.) %ld\n", h[1]);
    DBG_PULSE("PulseProcTzxDirectRecording: Used bits (samples) in last byte of data (1-8) %ld\n", h[2]);
    DBG_PULSE("PulseProcTzxDirectRecording: Length of samples' data %ld\n", h[3]);

    _data->init(
      _end,
      h[3],  // 3. BYTE[3]	Length of samples' data
      h[0],  // 0. WORD	Number of T-states per sample (bit of data)
      h[2]   // 2. BYTE	Used bits (samples) in last byte of data (1-8)
    );

    _end->init(_pause, 0, 1);

    _pause->init(
      next(),
       h[3], // 3. WORD	Pause after this block (ms.) {1000}
       _tsPerMs
    );    

    *top = _data;
    return PP_CONTINUE;
  }
}
