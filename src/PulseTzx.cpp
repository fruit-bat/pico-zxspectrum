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
  }
  _is = is;
  
  
  

}

/**
 * 0x00	-	WORD	Pause after this block (ms.) {1000}
 * 0x02	N	WORD	Length of data that follow
 * 0x04	-	BYTE[N]	Data as in .TAP files
 */
void PulseTzx::skipStandardSpeedData(){
  const int8_t l[] = {-2, 2};
  uint32_t h[1];
  int32_t r;
  r = _is->decodeLsbf(h, l, 2);
  if (r < 0) {
    // TODO error
  }
  else {
    r = _is->rseek(h[0]);
    if (r < 0) {
      // TODO error
    }    
  }
}

/**
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
void PulseTzx::skipTurboSpeedData(){
  const int8_t l[] = {-0xf, 3};
  uint32_t h[1];
  int32_t r;
  r = _is->decodeLsbf(h, l, 2);
  if (r < 0) {
    // TODO error
  }
  else {
    r = _is->rseek(h[0]);
    if (r < 0) {
      // TODO error
    }    
  }
}

void PulseTzx::skipPureTone(){

}

void PulseTzx::skipSequence(){

}

void PulseTzx::skipPureData(){

}

void PulseTzx::skipDirectRecording(){

}

void PulseTzx::skipCswRecording(){

}

void PulseTzx::skipGeneralizedData(){

}

void PulseTzx::skipPause(){

}

void PulseTzx::skipGroupStart(){

}

void PulseTzx::skipGroupEnd(){

}

void PulseTzx::skipJump(){

}

void PulseTzx::skipLoopStart(){

}

void PulseTzx::skipLoopEnd(){

}

void PulseTzx::skipCallSequence(){

}

void PulseTzx::skipReturnFromSequence(){

}

void PulseTzx::skipSelectBlock(){

}

void PulseTzx::skipStopTheTape48k(){

}

void PulseTzx::skipSetSignalLevel(){

}

void PulseTzx::skipTextDescription(){

}

void PulseTzx::skipMessage(){

}

void PulseTzx::skipArchiveInfo(){

}

void PulseTzx::skipHardwareType(){

}

void PulseTzx::skipCustomInfo(){

}

void PulseTzx::skipGlue(){

}



void PulseTzx::indexBlock(int32_t bt) {
  uint32_t pos = _is->pos();
  printf("TZX: Indexing block type %ld at %ld\n", bt, pos);
  _bi.push_back(pos);
  switch(bt) {
    // ID 10 - Standard speed data block
    case 0x10: skipStandardSpeedData(); break;
    // ID 11 - Turbo speed data block
    case 0x11: skipTurboSpeedData(); break;
    // ID 12 - Pure tone
    case 0x12: skipPureTone(); break;
    // ID 13 - Sequence of pulses of various lengths
    case 0x13: skipSequence(); break;
    // ID 14 - Pure data block
    case 0x14: skipPureData(); break;
    // ID 15 - Direct recording block
    case 0x15: skipDirectRecording(); break;
    // ID 18 - CSW recording block
    case 0x18: skipCswRecording(); break;
    // ID 19 - Generalized data block
    case 0x19: skipGeneralizedData(); break;
    // ID 20 - Pause (silence) or 'Stop the tape' command
    case 0x20: skipPause(); break;
    // ID 21 - Group start
    case 0x21: skipGroupStart(); break;
    // ID 22 - Group end
    case 0x22: skipGroupEnd(); break;
    // ID 23 - Jump to block
    case 0x23: skipJump(); break;
    // ID 24 - Loop start
    case 0x24: skipLoopStart(); break;
    // ID 25 - Loop end
    case 0x25: skipLoopEnd(); break;
    // ID 26 - Call sequence
    case 0x26: skipCallSequence(); break;
    // ID 27 - Return from sequence
    case 0x27: skipReturnFromSequence(); break;
    // ID 28 - Select block
    case 0x28: skipSelectBlock(); break;
    // ID 2A - Stop the tape if in 48K mode
    case 0x2a: skipStopTheTape48k(); break;
    // ID 2B - Set signal level
    case 0x2b: skipSetSignalLevel(); break;
    // ID 30 - Text description
    case 0x30: skipTextDescription(); break;
    // ID 31 - Message block
    case 0x31: skipMessage(); break;
    // ID 32 - Archive info
    case 0x32: skipArchiveInfo(); break;
    // ID 33 - Hardware type
    case 0x33: skipHardwareType(); break;
    // ID 35 - Custom info block
    case 0x35: skipCustomInfo(); break;
    // ID 5A - "Glue" block (90 dec, ASCII Letter 'Z')    
    case 0x5a: skipGlue(); break;

    default:
      // TODO error
      printf("TZX: Error unknown block type %ld\n", bt);
      break;
  }
}

void PulseTzx::indexBlocks() {
  printf("TZX: Indexing blocks...\n");
  
  // Make sure we are at the start of the blocks
  _is->seek(10);
  
  _bi.clear();
  
  while (true) {
    int32_t bt = _is->readByte();
    printf("TZX: Read block type %ld\n", bt);
    if (bt == -1) {
      _is->seek(10);
      return;
    }
    if (bt < 0) {
      printf("TZX: Error reading block type\n");
      return; // TODO error
    }
    indexBlock(bt);
  }
}


