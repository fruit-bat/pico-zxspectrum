#include "PulseTzx.h"


PulseTzx::PulseTzx() :
  _is(0)
{}

bool PulseTzx::end() {
  return (_is == 0);
}

void PulseTzx::reset(InputStream* is) {
  if ((_is != 0) && (_is != is)) _is->close();
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

/** ID 10 - Standard Speed Data Block
 * 0x00	-	WORD	Pause after this block (ms.) {1000}
 * 0x02	N	WORD	Length of data that follow
 * 0x04	-	BYTE[N]	Data as in .TAP files
 */
int32_t PulseTzx::skipStandardSpeedData(){
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
int32_t PulseTzx::skipTurboSpeedData(){
  const int8_t l[] = {-0xf, 3};
  return skipSingle(l, 2, 1);
}

/**
 * 0x00	-	WORD	Length of one pulse in T-states
 * 0x02	-	WORD	Number of pulses
 */
int32_t PulseTzx::skipPureTone(){
  return _is->rseek(4) < 0 ? -2 : 0;
}

/**
 * 0x00	N	BYTE	Number of pulses
 * 0x01	-	WORD[N]	Pulses' lengths
 */
int32_t PulseTzx::skipSequence(){
  const int8_t l[] = {1};
  return skipSingle(l, 1, 2);
}

/**
 * 0x00	-	WORD	Length of ZERO bit pulse
 * 0x02	-	WORD	Length of ONE bit pulse
 * 0x04	-	BYTE	Used bits in last byte (other bits should be 0)
 * (e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
 * 0x05	-	WORD	Pause after this block (ms.)
 * 0x07	N	BYTE[3]	Length of data that follow
 * 0x0A	-	BYTE[N]	Data as in .TAP files
 */
int32_t PulseTzx::skipPureData(){
  const int8_t l[] = {-0x7, 3};
  return skipSingle(l, 2, 1);
}

/**
 * 0x00	-	WORD	Number of T-states per sample (bit of data)
 * 0x02	-	WORD	Pause after this block in milliseconds (ms.)
 * 0x04	-	BYTE	Used bits (samples) in last byte of data (1-8)
 * (e.g. if this is 2, only first two samples of the last byte will be played)
 * 0x05	N	BYTE[3]	Length of samples' data
 * 0x08	-	BYTE[N]	Samples data. Each bit represents a state on the EAR port (i.e. one sample).
 * MSb is played first.
 */
int32_t PulseTzx::skipDirectRecording(){
  const int8_t l[] = {-0x5, 3};
  return skipSingle(l, 2, 1);
}

/**
 * 0x00	10+N	DWORD	Block length (without these four bytes)
 * 0x04	-	WORD	Pause after this block (in ms).
 * 0x06	-	BYTE[3]	Sampling rate
 * 0x09	-	BYTE	Compression type
 * 0x01: RLE
 * 0x02: Z-RLE
 * 0x0A	-	DWORD	Number of stored pulses (after decompression, for validation purposes)
 * 0x0E	-	BYTE[N]	CSW data, encoded according to the CSW file format specification.
 */
int32_t PulseTzx::skipCswRecording(){
  const int8_t l[] = {4};
  return skipSingle(l, 1, 1);
}

int32_t PulseTzx::skipGeneralizedData(){
  return -2;
}

int32_t PulseTzx::skipPause(){
  return -2;
}

int32_t PulseTzx::skipGroupStart(){
  return -2;
}

int32_t PulseTzx::skipGroupEnd(){
  return -2;
}

int32_t PulseTzx::skipJump(){
  return -2;
}

int32_t PulseTzx::skipLoopStart(){
  return -2;
}

int32_t PulseTzx::skipLoopEnd(){
  return -2;
}

int32_t PulseTzx::skipCallSequence(){
  return -2;
}

int32_t PulseTzx::skipReturnFromSequence(){
  return -2;
}

int32_t PulseTzx::skipSelectBlock(){
  return -2;
}

int32_t PulseTzx::skipStopTheTape48k(){
  return -2;
}

int32_t PulseTzx::skipSetSignalLevel(){
  return -2;
}

/** 0x30
 * 0x00	N	BYTE	Length of the text description
 * 0x01	-	CHAR[N]	Text description in ASCII format
 */
int32_t PulseTzx::skipTextDescription(){
  const int8_t l[] = {1};
  return skipSingle(l, 1, 1);
}

/** 0x31
 * 0x00	-	BYTE	Time (in seconds) for which the message should be displayed
 * 0x01	N	BYTE	Length of the text message
 * 0x02	-	CHAR[N]	Message that should be displayed in ASCII format
 */
int32_t PulseTzx::skipMessage(){
  const int8_t l[] = {-1, 1};
  return skipSingle(l, 2, 1);
}

int32_t PulseTzx::skipArchiveInfo(){
  return -2;
}

int32_t PulseTzx::skipHardwareType(){
  return -2;
}

int32_t PulseTzx::skipCustomInfo(){
  return -2;
}

int32_t PulseTzx::skipGlue(){
  return -2;
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


