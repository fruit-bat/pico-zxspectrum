#include "PulseProcTzxBlock.h"

PulseProcTzxBlock::PulseProcTzxBlock(
  PulseProcTap* ppTap,
  PulseProcStdHeader* header,
  PulseProcStdByteStream* data,
  PulseProcTone* ppTone1,
  PulseProcPauseMillis* ppPauseMillis  
) :
  _bi(0),
  _i(0),
  _loopStart(0),
  _loopCount(0),
  _ppTap(ppTap),
  _ppTzxTurbo(header, data, ppPauseMillis),
  _ppTzxPureTone(ppTone1),
  _ppPulseStream(ppTone1),
  _ppTzxPulseSequence(&_ppPulseStream),
  _ppTzxPureData(data, ppPauseMillis),
  _ppBitStream(),
  _ppTzxDirectRecording(&_ppBitStream, ppPauseMillis),
  _ppCallStream(&_i),
  _ppTzxGlue(),
  _ppPauseMillis(ppPauseMillis),
  _ppTzxGenData(ppPauseMillis),
  _ppTzxSelect(),
  _tsPerMs(3555)
{}
  
void PulseProcTzxBlock::init(
    PulseProc *nxt,
    std::vector<uint32_t>* bi,
    uint32_t tsPerMs,
    bool is48k
) {
  next(nxt);
  _bi = bi;
  _i = 0;
  _tsPerMs = tsPerMs;
  _loopStart = 0;
  _loopCount = 0;
  _is48k = is48k;
}

int32_t PulseProcTzxBlock::skipSingle(InputStream *is, const int8_t* l, uint32_t n, uint32_t m) {
  uint32_t h;
  return is->decodeLsbf(&h, l, n) < 0 || is->rseek(h * m) < 0 ? PP_ERROR : PP_CONTINUE;
}

int32_t PulseProcTzxBlock::skipOnly(InputStream *is, uint32_t n) {
  return is->rseek(n) < 0 ? PP_ERROR : PP_CONTINUE;
}

/** ID 10 - Standard Speed Data Block
 * 0x00	-	WORD	Pause after this block (ms.) {1000}
 * 0x02	N	WORD	Length of data that follow
 * 0x04	-	BYTE[N]	Data as in .TAP files
 */
int32_t PulseProcTzxBlock::doStandardSpeedData(InputStream *is, PulseProc **top) {
  const int8_t l[] = {2};
  uint32_t delay;
  if (is->decodeLsbf(&delay, l, 1) < 0) {
    DBG_PULSE("PulseProcTzxBlock: failed to read standard data delay\n");
    return PP_ERROR;
  }
  DBG_PULSE("PulseProcTzxBlock: standard block delay %ld ms\n", delay);  
  _ppTap->init(this, delay, _tsPerMs);
  *top = _ppTap;
  return PP_CONTINUE;
}

/** ID 11 - Turbo Speed Data Block
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
int32_t PulseProcTzxBlock::doTurboSpeedData(InputStream *is, PulseProc **top) {
  _ppTzxTurbo.init(this, _tsPerMs);
  *top = &_ppTzxTurbo;
  return PP_CONTINUE;
}

/** ID 12 - Pure Tone
 * 0x00	-	WORD	Length of one pulse in T-states
 * 0x02	-	WORD	Number of pulses
 */
int32_t PulseProcTzxBlock::doPureTone(InputStream *is, PulseProc **top) {
  _ppTzxPureTone.init(this);
  *top = &_ppTzxPureTone;
  return PP_CONTINUE;
}

/** ID 13 - Pulse sequence
 * 0x00	N	BYTE	Number of pulses
 * 0x01	-	WORD[N]	Pulses' lengths
 */
int32_t PulseProcTzxBlock::doSequence(InputStream *is, PulseProc **top) {
  _ppTzxPulseSequence.init(this);
  *top = &_ppTzxPulseSequence;
  return PP_CONTINUE;
}

/** ID 14 - Pure Data Block
 * 0x00	-	WORD	Length of ZERO bit pulse
 * 0x02	-	WORD	Length of ONE bit pulse
 * 0x04	-	BYTE	Used bits in last byte (other bits should be 0)
 * (e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
 * 0x05	-	WORD	Pause after this block (ms.)
 * 0x07	N	BYTE[3]	Length of data that follow
 * 0x0A	-	BYTE[N]	Data as in .TAP files
 */
int32_t PulseProcTzxBlock::doPureData(InputStream *is, PulseProc **top) {
  _ppTzxPureData.init(this, _tsPerMs);
  *top = &_ppTzxPureData;
  return PP_CONTINUE;
}

/** ID 15 - Direct Recording
 * 0x00	-	WORD	Number of T-states per sample (bit of data)
 * 0x02	-	WORD	Pause after this block in milliseconds (ms.)
 * 0x04	-	BYTE	Used bits (samples) in last byte of data (1-8)
 * (e.g. if this is 2, only first two samples of the last byte will be played)
 * 0x05	N	BYTE[3]	Length of samples' data
 * 0x08	-	BYTE[N]	Samples data. Each bit represents a state on the EAR port (i.e. one sample).
 * MSb is played first.
 */
int32_t PulseProcTzxBlock::doDirectRecording(InputStream *is, PulseProc **top) {
  _ppTzxDirectRecording.init(this, _tsPerMs);
  *top = &_ppTzxDirectRecording;
  return PP_CONTINUE;
}

/** ID 18 - CSW Recording
 * 0x00	10+N	DWORD	Block length (without these four bytes)
 * 0x04	-	WORD	Pause after this block (in ms).
 * 0x06	-	BYTE[3]	Sampling rate
 * 0x09	-	BYTE	Compression type
 * 0x01: RLE
 * 0x02: Z-RLE
 * 0x0A	-	DWORD	Number of stored pulses (after decompression, for validation purposes)
 * 0x0E	-	BYTE[N]	CSW data, encoded according to the CSW file format specification.
 */
int32_t PulseProcTzxBlock::doCswRecording(InputStream *is, PulseProc **top) {
  const int8_t l[] = {4};
  return skipSingle(is, l, 1, 1);
}

/** ID 19 - Generalized Data Block
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
int32_t PulseProcTzxBlock::doGeneralizedData(InputStream *is, PulseProc **top) {
  _ppTzxGenData.init(this, _tsPerMs);
  *top = &_ppTzxGenData;
  return PP_CONTINUE;
}

/** ID 20 - Pause (silence) or 'Stop the Tape' command
 * 0x00	-	WORD	Pause duration (ms.)
 */
int32_t PulseProcTzxBlock::doPause(InputStream *is, PulseProc **top) {
  const int8_t l[] = {2};
  uint32_t delay;
  if (is->decodeLsbf(&delay, l, 1) < 0) {
    DBG_PULSE("PulseProcTzxBlock: failed to read pause delay\n");
    return PP_ERROR;
  }
  DBG_PULSE("PulseProcTzxBlock: pause delay %ld ms\n", delay);
  if (delay == 0) {
    DBG_PULSE("PulseProcTzxBlock: pausing the tape\n");
    return PP_PAUSE;
  }
  else {
    _ppPauseMillis->init(this, delay, _tsPerMs);
    *top = _ppPauseMillis;
    return PP_CONTINUE;
  }
}

/** ID 21 - Group start
 * 0x00	L	BYTE	Length of the group name string
 * 0x01	-	CHAR[L]	Group name in ASCII format (please keep it under 30 characters long)
 */
int32_t PulseProcTzxBlock::doGroupStart(InputStream *is, PulseProc **top) {
  const int8_t l[] = {1};
  return skipSingle(is, l, 1, 1);
}

/** ID 22 - Group end
 */
int32_t PulseProcTzxBlock::doGroupEnd(InputStream *is, PulseProc **top) {
  return 0;
}

/** ID 23 - Jump to block
 * 0x00	-	WORD	Relative jump value
 */
int32_t PulseProcTzxBlock::doJump(InputStream *is, PulseProc **top) {
  const int8_t l[] = {2};
  uint32_t uoffset;
  if (is->decodeLsbf(&uoffset, l, 1) < 0) {
    DBG_PULSE("PulseProcTzxBlock: failed to read jump offset\n");
    return PP_ERROR;
  }
  int16_t offset = static_cast<int16_t>(uoffset);
  DBG_PULSE("PulseProcTzxBlock: do jump %d ms\n", offset);
  if (offset != 0) {
    _i += offset - 1;
  }
  else {
    DBG_PULSE("PulseProcTzxBlock: invalid jump offset %d ms\n", offset);
    return PP_ERROR;
  }
  return PP_CONTINUE;
}

/** ID 24 - Loop start
 * 0x00	-	WORD	Number of repetitions (greater than 1)
 */
int32_t PulseProcTzxBlock::doLoopStart(InputStream *is, PulseProc **top) {
  if (_loopCount > 0) {
    DBG_PULSE("PulseProcTzxBlock: don't nest loops\n");
    return PP_ERROR;
  }  
  const int8_t l[] = {2};
  if (is->decodeLsbf(&_loopCount, l, 1) < 0) {
    DBG_PULSE("PulseProcTzxBlock: failed to read loop count\n");
    return PP_ERROR;
  }
  _loopStart = _i;
  DBG_PULSE("PulseProcTzxBlock: loop count %ld ms\n", _loopCount);
  return PP_CONTINUE;
}

/** ID 25 - Loop end
 */
int32_t PulseProcTzxBlock::doLoopEnd(InputStream *is, PulseProc **top) {
  if (_loopCount > 0) {
    DBG_PULSE("PulseProcTzxBlock: loop end\n");
    --_loopCount;
    _i = _loopStart;
  }
  return PP_CONTINUE;
}

/** ID 26 - Call sequence
 * 0x00	N	WORD	Number of calls to be made
 * 0x02	-	WORD[N]	Array of call block numbers (relative-signed offsets)
 */
int32_t PulseProcTzxBlock::doCallSequence(InputStream *is, PulseProc **top) {
  const int8_t l[] = {2};
  uint32_t n;
  if (is->decodeLsbf(&n, l, 1) < 0) {
    DBG_PULSE("PulseProcTzxBlock: failed to read call sequence length\n");
    return PP_ERROR;
  }
  _ppCallStream.init(this, n, is->pos());
  *top = &_ppCallStream;
  return PP_CONTINUE;
}

/** ID 27 - Return from sequence
 */
int32_t PulseProcTzxBlock::doReturnFromSequence(InputStream *is, PulseProc **top) {
  *top = &_ppCallStream;
  return PP_CONTINUE;
}

/** ID 28 - Select block
 * 0x00	-	WORD	Length of the whole block (without these two bytes)
 * 0x02	N	BYTE	Number of selections
 * 0x03	-	SELECT[N]	List of selections
 */
int32_t PulseProcTzxBlock::doSelectBlock(InputStream *is, PulseProc **top) {
  DBG_PULSE("PulseProcTzxBlock: ID 28 - Select block NOT IMPLEMENTED!\n");
  _ppTzxSelect.init(this);
  *top = &_ppTzxSelect;
  return PP_CONTINUE;
}

/** ID 2A - Stop the tape if in 48K mode
 * 0x00	0	DWORD	Length of the block without these four bytes (0)
 */
int32_t PulseProcTzxBlock::doStopTheTape48k(InputStream *is, PulseProc **top) {
  if (_is48k) {
    DBG_PULSE("PulseProcTzxBlock: pausing the tape for 48k Spectrum\n");
    return PP_PAUSE;
  }
  else {
    return PP_CONTINUE;
  }
}

/** ID 2B - Set signal level
 * 0x00	1	DWORD	Block length (without these four bytes)
 * 0x04	-	BYTE	Signal level (0=low, 1=high)
 */
int32_t PulseProcTzxBlock::doSetSignalLevel(InputStream *is, PulseProc **top) {
  return skipOnly(is, 5);
}

/** ID 30 - Text description
 * 0x00	N	BYTE	Length of the text description
 * 0x01	-	CHAR[N]	Text description in ASCII format
 */
int32_t PulseProcTzxBlock::doTextDescription(InputStream *is, PulseProc **top) {
  const int8_t l[] = {1};
  return skipSingle(is, l, 1, 1);
}

/** ID 31 - Message block
 * 0x00	-	BYTE	Time (in seconds) for which the message should be displayed
 * 0x01	N	BYTE	Length of the text message
 * 0x02	-	CHAR[N]	Message that should be displayed in ASCII format
 */
int32_t PulseProcTzxBlock::doMessage(InputStream *is, PulseProc **top) {
  const int8_t l[] = {-1, 1};
  return skipSingle(is, l, 2, 1);
}

/** 32 - Archive info
 * 0x00	-	WORD	Length of the whole block (without these two bytes)
 * 0x02	N	BYTE	Number of text strings
 * 0x03	-	TEXT[N]	List of text strings
 */
int32_t PulseProcTzxBlock::doArchiveInfo(InputStream *is, PulseProc **top) {
  const int8_t l[] = {2};
  return skipSingle(is, l, 1, 1);
}

/** ID 33 - Hardware type
 * 0x00	N	BYTE	Number of machines and hardware types for which info is supplied
 * 0x01	-	HWINFO[N]	List of machines and hardware
 */
int32_t PulseProcTzxBlock::doHardwareType(InputStream *is, PulseProc **top) {
  const int8_t l[] = {1};
  return skipSingle(is, l, 1, 3);
}

/** ID 35 - Custom info block
 * 0x00	-	CHAR[10]	Identification string (in ASCII)
 * 0x10	L	DWORD	Length of the custom info
 * 0x14	-	BYTE[L]	Custom info
 */
int32_t PulseProcTzxBlock::doCustomInfo(InputStream *is, PulseProc **top) {
  const int8_t l[] = {-0x10, 4};
  return skipSingle(is, l, 2, 1);
}

/** ID 5A - "Glue" block
 * 0x00	-	BYTE[9]	Value: { "XTape!",0x1A,MajR,MinR }
 *        Just do these 9 bytes and you will end up on the next ID.
 */
int32_t PulseProcTzxBlock::doGlue(InputStream *is, PulseProc **top) {
  _ppTzxGlue.init(this);
  *top = &_ppTzxGlue;
  return PP_CONTINUE;
}


int32_t PulseProcTzxBlock::doBlock(InputStream *is, int32_t bt, PulseProc **top) {
  DBG_PULSE("PulseProcTzxBlock: reading block type %02lX at %ld\n", bt, is->pos() - 1);
  switch(bt) {
    // ID 10 - Standard speed data block
    case 0x10: return doStandardSpeedData(is, top); 
    // ID 11 - Turbo speed data block
    case 0x11: return doTurboSpeedData(is, top); 
    // ID 12 - Pure tone
    case 0x12: return doPureTone(is, top); 
    // ID 13 - Sequence of pulses of various lengths
    case 0x13: return doSequence(is, top); 
    // ID 14 - Pure data block
    case 0x14: return doPureData(is, top); 
    // ID 15 - Direct recording block
    case 0x15: return doDirectRecording(is, top); 
    // ID 18 - CSW recording block
    case 0x18: return doCswRecording(is, top); 
    // ID 19 - Generalized data block
    case 0x19: return doGeneralizedData(is, top); 
    // ID 20 - Pause (silence) or 'Stop the tape' command
    case 0x20: return doPause(is, top); 
    // ID 21 - Group start
    case 0x21: return doGroupStart(is, top); 
    // ID 22 - Group end
    case 0x22: return doGroupEnd(is, top); 
    // ID 23 - Jump to block
    case 0x23: return doJump(is, top); 
    // ID 24 - Loop start
    case 0x24: return doLoopStart(is, top); 
    // ID 25 - Loop end
    case 0x25: return doLoopEnd(is, top); 
    // ID 26 - Call sequence
    case 0x26: return doCallSequence(is, top); 
    // ID 27 - Return from sequence
    case 0x27: return doReturnFromSequence(is, top); 
    // ID 28 - Select block
    case 0x28: return doSelectBlock(is, top); 
    // ID 2A - Stop the tape if in 48K mode
    case 0x2a: return doStopTheTape48k(is, top); 
    // ID 2B - Set signal level
    case 0x2b: return doSetSignalLevel(is, top); 
    // ID 30 - Text description
    case 0x30: return doTextDescription(is, top); 
    // ID 31 - Message block
    case 0x31: return doMessage(is, top); 
    // ID 32 - Archive info
    case 0x32: return doArchiveInfo(is, top); 
    // ID 33 - Hardware type
    case 0x33: return doHardwareType(is, top); 
    // ID 35 - Custom info block
    case 0x35: return doCustomInfo(is, top); 
    // ID 5A - "Glue" block (90 dec, ASCII Letter 'Z')    
    case 0x5a: return doGlue(is, top); 

    default:
      DBG_PULSE("PulseProcTzxBlock: Error unknown block type %02lX\n", bt);
      return PP_ERROR;
  }
}

int32_t PulseProcTzxBlock::advance(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  if (_i < _bi->size()) {
    DBG_PULSE("PulseProcTzxBlock: seek to block %ld\n", _i);
    uint32_t p = _bi->at(_i++);
    int32_t r;
    uint32_t a = is->pos();
    if (a != p) {
      r = is->seek(p);
      if (r < 0) {
        DBG_PULSE("PulseProcTzxBlock: failed to seek to block\n");
        return PP_ERROR;
      }
    }
    int32_t bt = is->readByte();
    DBG_PULSE("PulseProcTzxBlock: Read block type %02lX\n", bt);
    if (bt < 0) {
      printf("PulseProcTzxBlock: Error reading block type\n");
      return PP_ERROR;
    }
    r = doBlock(is, bt, top);
    DBG_PULSE("PulseProcTzxBlock: do block type %02lX returned %ld\n", bt, r);
    return r;
  }
  else {
    return PP_COMPLETE;
  }
}

void PulseProcTzxBlock::option(uint32_t option) {
  uint32_t offset = _ppTzxSelect.offset(option);
  _i += offset - 1;
}
