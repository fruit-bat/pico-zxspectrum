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

  }
  _is = is;
  
  
  
  // TODO remove later
  indexBlocks();
  _is->close();
  _is = 0;
}

void PulseTzx::skipStandardSpeedData(){

}

void PulseTzx::skipTurboSpeedData(){

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



void PulseTzx::indexBlock(int bt) {
  unsigned int pos = _is->pos();
  printf("TZX: Indexing block type %d at %d\n", bt, pos);
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
      printf("TZX: Error unknown block type %d\n", bt);
      break;
  }
}

void PulseTzx::indexBlocks() {
  printf("TZX: Indexing blocks...\n");
  
  // Make sure we are at the start of the blocks
  _is->seek(10);
  
  _bi.clear();
  
  while (true) {
    int bt = _is->readByte();
    printf("TZX: Read block type %d\n", bt);
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


