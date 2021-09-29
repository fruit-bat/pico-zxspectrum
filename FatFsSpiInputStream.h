#pragma once

#include "InputStream.h"
#include "SdCardFatFsSpi.h"
#include "f_util.h"

class FatFsSpiInputStream : public InputStream {
  SdCardFatFsSpi* _sdCard;
  FRESULT _fr;     /* Return value for file */
  FIL _fil;        /* File object */
  bool _eof;
  bool _open;
public:
  FatFsSpiInputStream(SdCardFatFsSpi* sdCard, const char* name);
  virtual int readByte();
  virtual int read(unsigned char* buffer, const unsigned int length); // returns actual length read, -1 for eof, -ve for error
  virtual void close();
};
