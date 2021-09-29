#pragma once

#include "InputStream.h"

class BufferedInputStream : public InputStream {
  InputStream* _delegate;
  unsigned char _buffer[1024];
  unsigned int _len;
  unsigned int _pos;
  void flood();
public:
  BufferedInputStream(InputStream* delegate);
  virtual int readByte(); // read a single byte, -1 for eof, -ve for error
  virtual void close();
};
