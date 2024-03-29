#include "PulseProcTzxGenData.h"


PulseProcTzxGenData::PulseProcTzxGenData(
  PulseProcPauseMillis* pause
) : 
  _sd(10),
  _pause(pause),
  _symdefsPilot(&_sd),
  _symbolPilot(&_symdefsPilot),
  _ppRle(&_symbolPilot),
  _symdefsData(&_sd),
  _symbolData(&_symdefsData),
  _ppSymData(&_symbolData)
{
}

void PulseProcTzxGenData::init(
  PulseProc *nxt,
  uint32_t tsPerMs
) {
  next(nxt);
  _tsPerMs = tsPerMs;
}

/**
 * ID 19 - Generalized Data Block
 * 
 * length: [00,01,02,03]+04
 * 
 * Offset	Value	Type	Description
 * 0x00	-	DWORD	Block length (without these four bytes)
 * 0x04	-	WORD	Pause after this block (ms)
 * 0x06	TOTP	DWORD	Total number of symbols in pilot/sync block (can be 0)
 * 0x0A	NPP	BYTE	Maximum number of pulses per pilot/sync symbol
 * 0x0B	ASP	BYTE	Number of pilot/sync symbols in the alphabet table (0=256)
 * 0x0C	TOTD	DWORD	Total number of symbols in data stream (can be 0)
 * 0x10	NPD	BYTE	Maximum number of pulses per data symbol
 * 0x11	ASD	BYTE	Number of data symbols in the alphabet table (0=256)
 * 0x12	-	SYMDEF[ASP]	Pilot and sync symbols definition table
 * This field is present only if TOTP>0
 * 0x12+
 * (2*NPP+1)*ASP	-	PRLE[TOTP]	Pilot and sync data stream
 * This field is present only if TOTP>0
 * 0x12+
 * (TOTP>0)*((2*NPP+1)*ASP)+
 * TOTP*3	-	SYMDEF[ASD]	Data symbols definition table
 * This field is present only if TOTD>0
 * 0x12+
 * (TOTP>0)*((2*NPP+1)*ASP)+
 * TOTP*3+
 * (2*NPD+1)*ASD	-	BYTE[DS]	Data stream
 * This field is present only if TOTD>0
*/
int32_t PulseProcTzxGenData::advance(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  DBG_PULSE("PulseProcTzxGenData: \n");
  
  const int8_t l[] = {
    4, // 0.      DWORD	Block length (without these four bytes)
    2, // 1.      WORD	Pause after this block (ms)
    4, // 2. TOTP	DWORD	Total number of symbols in pilot/sync block (can be 0)
    1, // 3. NPP	BYTE	Maximum number of pulses per pilot/sync symbol
    1, // 4. ASP	BYTE	Number of pilot/sync symbols in the alphabet table (0=256)
    4, // 5, TOTD	DWORD	Total number of symbols in data stream (can be 0)
    1, // 6. NPD	BYTE	Maximum number of pulses per data symbol
    1  // 7. ASD	BYTE	Number of data symbols in the alphabet table (0=256)
  };
  
  uint32_t h[8];
  int32_t r = is->decodeLsbf(h, l, 8);
  
  if (r < 0) {
    DBG_PULSE("PulseProcTzxGenData: Error (%ld) reading pure data header\n", r);
    return PP_ERROR;
  }
    
  DBG_PULSE("PulseProcTzxGenData: Block length (without these four bytes) %ld\n", h[0]);
  DBG_PULSE("PulseProcTzxGenData: Pause after this block (ms) %ld\n", h[1]);
  DBG_PULSE("PulseProcTzxGenData: Total number of symbols in pilot/sync block %ld\n", h[2]);
  DBG_PULSE("PulseProcTzxGenData: Maximum number of pulses per pilot/sync symbol %ld\n", h[3]);
  DBG_PULSE("PulseProcTzxGenData: Number of pilot/sync symbols in the alphabet table %ld\n", h[4]);
  DBG_PULSE("PulseProcTzxGenData: Total number of symbols in data stream %ld\n", h[5]);
  DBG_PULSE("PulseProcTzxGenData: Maximum number of pulses per data symbol %ld\n", h[6]);
  DBG_PULSE("PulseProcTzxGenData: Number of data symbols in the alphabet table %ld\n", h[7]);

    
  bool hasPilot = h[2] > 0;
  bool hasData = h[5] > 0;
  
  uint32_t bps = calcBps(h[7]);
  DBG_PULSE("PulseProcTzxGenData: Calculated %ld bits required for %ld symbols\n", bps, h[7]);
  if (hasData && h[7] < 2) {
    DBG_PULSE("PulseProcTzxGenData: Error not enough data symbol definitions %ld\n", h[7]);
    return PP_ERROR;
  }

  if (hasPilot) {
    _symdefsPilot.init(
      &_ppRle,
      h[3], // 3. NPP	BYTE	Maximum number of pulses per pilot/sync symbol
      h[4]  // 4. ASP	BYTE	Number of pilot/sync symbols in the alphabet table (0=256)
    );
    
    _ppRle.init(
      hasData ? (PulseProc*)&_symdefsData : (PulseProc*)_pause,
      h[2]  // 2. TOTP	DWORD	Total number of symbols in pilot/sync block (can be 0)
    );
  }
  
  if (hasData) {
    _symdefsData.init(
      &_ppSymData,
      h[6], // 6. NPD	BYTE	Maximum number of pulses per data symbol
      h[7]  // 7. ASD	BYTE	Number of data symbols in the alphabet table (0=256)
    );
    
    _ppSymData.init(
      _pause,
      h[5],  // 5, TOTD	DWORD	Total number of symbols in data stream (can be 0)
      bps
    );
  }
        
  _pause->init(
    next(),
     h[1],   // 1. WORD	Pause after this block (ms)
     _tsPerMs
  );    

  *top = hasPilot ? (PulseProc*)&_symdefsPilot : hasData ? (PulseProc*)&_symdefsData : (PulseProc*)_pause;
  return PP_CONTINUE;
}

uint32_t PulseProcTzxGenData::calcBps(uint32_t ns) {
  if (ns == 0) return 8;
  for (uint32_t i = 0; i < 8; ++i) {
    if (ns <= (1UL << i)) return i;
  }
  return 8;
}
