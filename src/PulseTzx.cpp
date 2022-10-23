#include "PulseTzx.h"


PulseTzx::PulseTzx() :
  _is(0)
{}

bool PulseTzx::end() {
  return (_is == 0);
}

void PulseTzx::reset(InputStream* is) {
  if (_is != 0) _is->close();
  if (is) {
    uint8_t header[10];
    
    printf("About to read TZX header\n");
    int l = is->read(header, 10);

    if (l < 10) {
      printf("Could not read complete TZX header\n");
      is->close();
      return;
    }
    
    if (strncmp((const char*)header, "ZXTape!", 7) != 0) {
      printf("Could not verify TZX signature bytes\n");
      is->close();
      return;
    }
    
    if (header[7] != 0x1a) {
      printf("Could not find end of text file marker\n");
      is->close();
      return;
    }
    
    uint8_t version_major = header[8];
    uint8_t version_minor = header[9];
    
    printf("Found TZX header version %d %d\n", version_major, version_minor);

    // TODO remove later
    _is = is;
    indexBlocks();
    _is->close();
    _is = 0;
    is = 0;
  }
  _is = is;
  
  
  

}

int32_t PulseTzx::skipSingle(const int8_t* l, uint32_t n, uint32_t m) {
  uint32_t h;
  return _is->decodeLsbf(&h, l, n) < 0 || _is->rseek(h * m) < 0 ? -2 : 0;
}

int32_t PulseTzx::skipOnly(uint32_t n) {
  return _is->rseek(n) < 0 ? -2 : 0;
}

/** ID 10 - Standard Speed Data Block
 * 0x00	-	WORD	Pause after this block (ms.) {1000}
 * 0x02	N	WORD	Length of data that follow
 * 0x04	-	BYTE[N]	Data as in .TAP files
 */
int32_t PulseTzx::skipStandardSpeedData() {
  const int8_t l[] = {-2, 2};
  return skipSingle(l, 2, 1);
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
int32_t PulseTzx::skipTurboSpeedData() {
  const int8_t l[] = {-0xf, 3};
  return skipSingle(l, 2, 1);
}

/** ID 12 - Pure Tone
 * 0x00	-	WORD	Length of one pulse in T-states
 * 0x02	-	WORD	Number of pulses
 */
int32_t PulseTzx::skipPureTone() {
  return skipOnly(4);
}

/** ID 13 - Pulse sequence
 * 0x00	N	BYTE	Number of pulses
 * 0x01	-	WORD[N]	Pulses' lengths
 */
int32_t PulseTzx::skipSequence() {
  const int8_t l[] = {1};
  return skipSingle(l, 1, 2);
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
int32_t PulseTzx::skipPureData() {
  const int8_t l[] = {-0x7, 3};
  return skipSingle(l, 2, 1);
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
int32_t PulseTzx::skipDirectRecording() {
  const int8_t l[] = {-0x5, 3};
  return skipSingle(l, 2, 1);
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
int32_t PulseTzx::skipCswRecording() {
  const int8_t l[] = {4};
  return skipSingle(l, 1, 1);
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
int32_t PulseTzx::skipGeneralizedData() {
  const int8_t l[] = {4};
  return skipSingle(l, 1, 1);
}

/** ID 20 - Pause (silence) or 'Stop the Tape' command
 * 0x00	-	WORD	Pause duration (ms.)
 */
int32_t PulseTzx::skipPause() {
  return skipOnly(2);
}

/** ID 21 - Group start
 * 0x00	L	BYTE	Length of the group name string
 * 0x01	-	CHAR[L]	Group name in ASCII format (please keep it under 30 characters long)
 */
int32_t PulseTzx::skipGroupStart() {
  const int8_t l[] = {1};
  return skipSingle(l, 1, 1);
}

/** ID 22 - Group end
 */
int32_t PulseTzx::skipGroupEnd() {
  return 0;
}

/** ID 23 - Jump to block
 * 0x00	-	WORD	Relative jump value
 */
int32_t PulseTzx::skipJump() {
  return skipOnly(2);
}

/** ID 24 - Loop start
 * 0x00	-	WORD	Number of repetitions (greater than 1)
 */
int32_t PulseTzx::skipLoopStart() {
  return skipOnly(2);
}

/** ID 25 - Loop end
 */
int32_t PulseTzx::skipLoopEnd() {
  return 0;
}

/** ID 26 - Call sequence
 * 0x00	N	WORD	Number of calls to be made
 * 0x02	-	WORD[N]	Array of call block numbers (relative-signed offsets)
 */
int32_t PulseTzx::skipCallSequence() {
  const int8_t l[] = {2};
  return skipSingle(l, 1, 2);
}

/** ID 27 - Return from sequence
 */
int32_t PulseTzx::skipReturnFromSequence() {
  return 0;
}

/** ID 28 - Select block
 * 0x00	-	WORD	Length of the whole block (without these two bytes)
 * 0x02	N	BYTE	Number of selections
 * 0x03	-	SELECT[N]	List of selections
 */
int32_t PulseTzx::skipSelectBlock() {
  const int8_t l[] = {2};
  return skipSingle(l, 1, 1);
}

/** ID 2A - Stop the tape if in 48K mode
 * 0x00	0	DWORD	Length of the block without these four bytes (0)
 */
int32_t PulseTzx::skipStopTheTape48k() {
  return skipOnly(4);
}

/** ID 2B - Set signal level
 * 0x00	1	DWORD	Block length (without these four bytes)
 * 0x04	-	BYTE	Signal level (0=low, 1=high)
 */
int32_t PulseTzx::skipSetSignalLevel() {
  return skipOnly(5);
}

/** ID 30 - Text description
 * 0x00	N	BYTE	Length of the text description
 * 0x01	-	CHAR[N]	Text description in ASCII format
 */
int32_t PulseTzx::skipTextDescription() {
  const int8_t l[] = {1};
  return skipSingle(l, 1, 1);
}

/** ID 31 - Message block
 * 0x00	-	BYTE	Time (in seconds) for which the message should be displayed
 * 0x01	N	BYTE	Length of the text message
 * 0x02	-	CHAR[N]	Message that should be displayed in ASCII format
 */
int32_t PulseTzx::skipMessage() {
  const int8_t l[] = {-1, 1};
  return skipSingle(l, 2, 1);
}

/** 32 - Archive info
 * 0x00	-	WORD	Length of the whole block (without these two bytes)
 * 0x02	N	BYTE	Number of text strings
 * 0x03	-	TEXT[N]	List of text strings
 */
int32_t PulseTzx::skipArchiveInfo() {
  const int8_t l[] = {2};
  return skipSingle(l, 1, 1);
}

/** ID 33 - Hardware type
 * 0x00	N	BYTE	Number of machines and hardware types for which info is supplied
 * 0x01	-	HWINFO[N]	List of machines and hardware
 */
int32_t PulseTzx::skipHardwareType() {
  const int8_t l[] = {1};
  return skipSingle(l, 1, 3);
}

/** ID 35 - Custom info block
 * 0x00	-	CHAR[10]	Identification string (in ASCII)
 * 0x10	L	DWORD	Length of the custom info
 * 0x14	-	BYTE[L]	Custom info
 */
int32_t PulseTzx::skipCustomInfo() {
  const int8_t l[] = {-10, 4};
  return skipSingle(l, 1, 1);
}

/** ID 5A - "Glue" block
 * 0x00	-	BYTE[9]	Value: { "XTape!",0x1A,MajR,MinR }
 *        Just skip these 9 bytes and you will end up on the next ID.
 */
int32_t PulseTzx::skipGlue() {
  return skipOnly(9);
}

int32_t PulseTzx::indexBlock(int32_t bt) {
  uint32_t pos = _is->pos() - 1;
  printf("TZX: Indexing block type %02lX at %ld\n", bt, pos);
  _bi.push_back(pos);
  switch(bt) {
    // ID 10 - Standard speed data block
    case 0x10: return skipStandardSpeedData(); 
    // ID 11 - Turbo speed data block
    case 0x11: return skipTurboSpeedData(); 
    // ID 12 - Pure tone
    case 0x12: return skipPureTone(); 
    // ID 13 - Sequence of pulses of various lengths
    case 0x13: return skipSequence(); 
    // ID 14 - Pure data block
    case 0x14: return skipPureData(); 
    // ID 15 - Direct recording block
    case 0x15: return skipDirectRecording(); 
    // ID 18 - CSW recording block
    case 0x18: return skipCswRecording(); 
    // ID 19 - Generalized data block
    case 0x19: return skipGeneralizedData(); 
    // ID 20 - Pause (silence) or 'Stop the tape' command
    case 0x20: return skipPause(); 
    // ID 21 - Group start
    case 0x21: return skipGroupStart(); 
    // ID 22 - Group end
    case 0x22: return skipGroupEnd(); 
    // ID 23 - Jump to block
    case 0x23: return skipJump(); 
    // ID 24 - Loop start
    case 0x24: return skipLoopStart(); 
    // ID 25 - Loop end
    case 0x25: return skipLoopEnd(); 
    // ID 26 - Call sequence
    case 0x26: return skipCallSequence(); 
    // ID 27 - Return from sequence
    case 0x27: return skipReturnFromSequence(); 
    // ID 28 - Select block
    case 0x28: return skipSelectBlock(); 
    // ID 2A - Stop the tape if in 48K mode
    case 0x2a: return skipStopTheTape48k(); 
    // ID 2B - Set signal level
    case 0x2b: return skipSetSignalLevel(); 
    // ID 30 - Text description
    case 0x30: return skipTextDescription(); 
    // ID 31 - Message block
    case 0x31: return skipMessage(); 
    // ID 32 - Archive info
    case 0x32: return skipArchiveInfo(); 
    // ID 33 - Hardware type
    case 0x33: return skipHardwareType(); 
    // ID 35 - Custom info block
    case 0x35: return skipCustomInfo(); 
    // ID 5A - "Glue" block (90 dec, ASCII Letter 'Z')    
    case 0x5a: return skipGlue(); 

    default:
      // TODO error
      printf("TZX: Error unknown block type %02lX\n", bt);
      return -4;
  }
}

int32_t PulseTzx::indexBlocks() {
  printf("TZX: Indexing blocks...\n");
  
  // Make sure we are at the start of the blocks
  _is->seek(10);
  
  _bi.clear();
  
  while (true) {
    int32_t bt = _is->readByte();
    printf("TZX: Read block type %02lX\n", bt);
    if (bt == -1) {
      _is->seek(10);
      return 0;
    }
    if (bt < 0) {
      printf("TZX: Error reading block type\n");
      return -3;
    }
    int32_t r = indexBlock(bt);
    printf("TZX: Indexing block type %02lX returned %ld\n", bt, r);
    
    if (r < 0) {
      printf("TZX: Error (%ld) indexing block type %02lX\n", r, bt);
      return r;
    }
  }
  
  return 0;
}


