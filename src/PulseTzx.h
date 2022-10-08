#pragma once

#include "PulseByte.h"
#include "PulseByteStream.h"
#include "PulsePreamble.h"
#include <cstring>

class PulseTzx {
  InputStream* _is;
  int _tstates;
public:
  PulseTzx() : _is(0) {}
  
  bool __not_in_flash_func(end)() {
    return (_is == 0);
  }
  
  void __not_in_flash_func(reset)(InputStream* is) {
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
      is->close();
      
    }
    // _is = is;
  }

  int __not_in_flash_func(advance)(int tstates, bool *pstate) {
    if (end()) return -1;
    _tstates += tstates;
 
    _is->close();
    return 0;
  }
};
