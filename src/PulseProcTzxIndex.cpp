#include "PulseProcTzxIndex.h"

PulseProcTzxIndex::PulseProcTzxIndex() : _bi(0)
{}
  
void PulseProcTzxIndex::init(PulseProc *nxt, std::vector<uint32_t>* bi) {
  next(nxt);
  _bi = bi;
}

int32_t PulseProcTzxIndex::skipSingle(InputStream *is, const int8_t* l, uint32_t n, uint32_t m) {
  uint32_t h;
  return is->decodeLsbf(&h, l, n) < 0 || is->rseek(h * m) < 0 ? PP_ERROR : PP_CONTINUE;
}

int32_t PulseProcTzxIndex::skipOnly(InputStream *is, uint32_t n) {
  return is->rseek(n) < 0 ? PP_ERROR : PP_CONTINUE;
}

/** ID 10 - Standard Speed Data Block
 * 0x00	-	WORD	Pause after this block (ms.) {1000}
 * 0x02	N	WORD	Length of data that follow
 * 0x04	-	BYTE[N]	Data as in .TAP files
 */
int32_t PulseProcTzxIndex::skipStandardSpeedData(InputStream *is) {
  const int8_t l[] = {-2, 2};
  return skipSingle(is, l, 2, 1);
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
int32_t PulseProcTzxIndex::skipTurboSpeedData(InputStream *is) {
  const int8_t l[] = {-0xf, 3};
  return skipSingle(is, l, 2, 1);
}

/** ID 12 - Pure Tone
 * 0x00	-	WORD	Length of one pulse in T-states
 * 0x02	-	WORD	Number of pulses
 */
int32_t PulseProcTzxIndex::skipPureTone(InputStream *is) {
  return skipOnly(is, 4);
}

/** ID 13 - Pulse sequence
 * 0x00	N	BYTE	Number of pulses
 * 0x01	-	WORD[N]	Pulses' lengths
 */
int32_t PulseProcTzxIndex::skipSequence(InputStream *is) {
  const int8_t l[] = {1};
  return skipSingle(is, l, 1, 2);
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
int32_t PulseProcTzxIndex::skipPureData(InputStream *is) {
  const int8_t l[] = {-0x7, 3};
  return skipSingle(is, l, 2, 1);
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
int32_t PulseProcTzxIndex::skipDirectRecording(InputStream *is) {
  const int8_t l[] = {-0x5, 3};
  return skipSingle(is, l, 2, 1);
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
int32_t PulseProcTzxIndex::skipCswRecording(InputStream *is) {
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
int32_t PulseProcTzxIndex::skipGeneralizedData(InputStream *is) {
  const int8_t l[] = {4};
  return skipSingle(is, l, 1, 1);
}

/** ID 20 - Pause (silence) or 'Stop the Tape' command
 * 0x00	-	WORD	Pause duration (ms.)
 */
int32_t PulseProcTzxIndex::skipPause(InputStream *is) {
  return skipOnly(is, 2);
}

/** ID 21 - Group start
 * 0x00	L	BYTE	Length of the group name string
 * 0x01	-	CHAR[L]	Group name in ASCII format (please keep it under 30 characters long)
 */
int32_t PulseProcTzxIndex::skipGroupStart(InputStream *is) {
  const int8_t l[] = {1};
  return skipSingle(is, l, 1, 1);
}

/** ID 22 - Group end
 */
int32_t PulseProcTzxIndex::skipGroupEnd(InputStream *is) {
  return 0;
}

/** ID 23 - Jump to block
 * 0x00	-	WORD	Relative jump value
 */
int32_t PulseProcTzxIndex::skipJump(InputStream *is) {
  return skipOnly(is, 2);
}

/** ID 24 - Loop start
 * 0x00	-	WORD	Number of repetitions (greater than 1)
 */
int32_t PulseProcTzxIndex::skipLoopStart(InputStream *is) {
  return skipOnly(is, 2);
}

/** ID 25 - Loop end
 */
int32_t PulseProcTzxIndex::skipLoopEnd(InputStream *is) {
  return 0;
}

/** ID 26 - Call sequence
 * 0x00	N	WORD	Number of calls to be made
 * 0x02	-	WORD[N]	Array of call block numbers (relative-signed offsets)
 */
int32_t PulseProcTzxIndex::skipCallSequence(InputStream *is) {
  const int8_t l[] = {2};
  return skipSingle(is, l, 1, 2);
}

/** ID 27 - Return from sequence
 */
int32_t PulseProcTzxIndex::skipReturnFromSequence(InputStream *is) {
  return 0;
}

/** ID 28 - Select block
 * 0x00	-	WORD	Length of the whole block (without these two bytes)
 * 0x02	N	BYTE	Number of selections
 * 0x03	-	SELECT[N]	List of selections
 */
int32_t PulseProcTzxIndex::skipSelectBlock(InputStream *is) {
  const int8_t l[] = {2};
  return skipSingle(is, l, 1, 1);
}

/** ID 2A - Stop the tape if in 48K mode
 * 0x00	0	DWORD	Length of the block without these four bytes (0)
 */
int32_t PulseProcTzxIndex::skipStopTheTape48k(InputStream *is) {
  return skipOnly(is, 4);
}

/** ID 2B - Set signal level
 * 0x00	1	DWORD	Block length (without these four bytes)
 * 0x04	-	BYTE	Signal level (0=low, 1=high)
 */
int32_t PulseProcTzxIndex::skipSetSignalLevel(InputStream *is) {
  return skipOnly(is, 5);
}

/** ID 30 - Text description
 * 0x00	N	BYTE	Length of the text description
 * 0x01	-	CHAR[N]	Text description in ASCII format
 */
int32_t PulseProcTzxIndex::skipTextDescription(InputStream *is) {
  const int8_t l[] = {1};
  return skipSingle(is, l, 1, 1);
}

/** ID 31 - Message block
 * 0x00	-	BYTE	Time (in seconds) for which the message should be displayed
 * 0x01	N	BYTE	Length of the text message
 * 0x02	-	CHAR[N]	Message that should be displayed in ASCII format
 */
int32_t PulseProcTzxIndex::skipMessage(InputStream *is) {
  const int8_t l[] = {-1, 1};
  return skipSingle(is, l, 2, 1);
}

/** 32 - Archive info
 * 0x00	-	WORD	Length of the whole block (without these two bytes)
 * 0x02	N	BYTE	Number of text strings
 * 0x03	-	TEXT[N]	List of text strings
 */
int32_t PulseProcTzxIndex::skipArchiveInfo(InputStream *is) {
  const int8_t l[] = {2};
  return skipSingle(is, l, 1, 1);
}

/** ID 33 - Hardware type
 * 0x00	N	BYTE	Number of machines and hardware types for which info is supplied
 * 0x01	-	HWINFO[N]	List of machines and hardware
 */
int32_t PulseProcTzxIndex::skipHardwareType(InputStream *is) {
  const int8_t l[] = {1};
  return skipSingle(is, l, 1, 3);
}

/** ID 35 - Custom info block
 * 0x00	-	CHAR[10]	Identification string (in ASCII)
 * 0x10	L	DWORD	Length of the custom info
 * 0x14	-	BYTE[L]	Custom info
 */
int32_t PulseProcTzxIndex::skipCustomInfo(InputStream *is) {
  const int8_t l[] = {-0x10, 4};
  return skipSingle(is, l, 2, 1);
}

/** ID 5A - "Glue" block
 * 0x00	-	BYTE[9]	Value: { "XTape!",0x1A,MajR,MinR }
 *        Just skip these 9 bytes and you will end up on the next ID.
 */
int32_t PulseProcTzxIndex::skipGlue(InputStream *is) {
  return skipOnly(is, 9);
}


int32_t PulseProcTzxIndex::indexBlock(InputStream *is, int32_t bt) {
  uint32_t pos = is->pos() - 1;
  DBG_PULSE("PulseProcTzxIndex: Indexing block type %02lX at %ld\n", bt, pos);
  _bi->push_back(pos);
  switch(bt) {
    // ID 10 - Standard speed data block
    case 0x10: return skipStandardSpeedData(is); 
    // ID 11 - Turbo speed data block
    case 0x11: return skipTurboSpeedData(is); 
    // ID 12 - Pure tone
    case 0x12: return skipPureTone(is); 
    // ID 13 - Sequence of pulses of various lengths
    case 0x13: return skipSequence(is); 
    // ID 14 - Pure data block
    case 0x14: return skipPureData(is); 
    // ID 15 - Direct recording block
    case 0x15: return skipDirectRecording(is); 
    // ID 18 - CSW recording block
    case 0x18: return skipCswRecording(is); 
    // ID 19 - Generalized data block
    case 0x19: return skipGeneralizedData(is); 
    // ID 20 - Pause (silence) or 'Stop the tape' command
    case 0x20: return skipPause(is); 
    // ID 21 - Group start
    case 0x21: return skipGroupStart(is); 
    // ID 22 - Group end
    case 0x22: return skipGroupEnd(is); 
    // ID 23 - Jump to block
    case 0x23: return skipJump(is); 
    // ID 24 - Loop start
    case 0x24: return skipLoopStart(is); 
    // ID 25 - Loop end
    case 0x25: return skipLoopEnd(is); 
    // ID 26 - Call sequence
    case 0x26: return skipCallSequence(is); 
    // ID 27 - Return from sequence
    case 0x27: return skipReturnFromSequence(is); 
    // ID 28 - Select block
    case 0x28: return skipSelectBlock(is); 
    // ID 2A - Stop the tape if in 48K mode
    case 0x2a: return skipStopTheTape48k(is); 
    // ID 2B - Set signal level
    case 0x2b: return skipSetSignalLevel(is); 
    // ID 30 - Text description
    case 0x30: return skipTextDescription(is); 
    // ID 31 - Message block
    case 0x31: return skipMessage(is); 
    // ID 32 - Archive info
    case 0x32: return skipArchiveInfo(is); 
    // ID 33 - Hardware type
    case 0x33: return skipHardwareType(is); 
    // ID 35 - Custom info block
    case 0x35: return skipCustomInfo(is); 
    // ID 5A - "Glue" block (90 dec, ASCII Letter 'Z')    
    case 0x5a: return skipGlue(is); 

    default:
      DBG_PULSE("PulseProcTzxIndex: Error unknown block type %02lX\n", bt);
      return PP_ERROR;
  }
}

int32_t PulseProcTzxIndex::advance(
  InputStream *is,
  bool *pstate,
  PulseProc **top
) {
  _bi->clear();
  
  while (true) {
    int32_t bt = is->readByte();
    DBG_PULSE("PulseProcTzxIndex: Read block type %02lX\n", bt);
    if (bt == -1) {
      DBG_PULSE("PulseProcTzxIndex: Complete\n");
      return PP_COMPLETE;
    }
    if (bt < 0) {
      DBG_PULSE("PulseProcTzxIndex: Error reading block type\n");
      return PP_ERROR;
    }
    int32_t r = indexBlock(is, bt);
    DBG_PULSE("PulseProcTzxIndex: Indexing block type %02lX returned %ld\n", bt, r);
    
    if (r < 0) {
      DBG_PULSE("PulseProcTzxIndex: Error (%ld) indexing block type %02lX\n", r, bt);
      return PP_ERROR;
    }
  }
}
