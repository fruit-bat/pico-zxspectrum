#pragma once

#include "OutputStream.h"

class SizingOutputStream : public OutputStream {
  unsigned int _l;
public:
  SizingOutputStream() : _l(0) {}
  virtual int writeByte(int b) { _l++; return 1; }
  virtual int writeWord(int w) { _l += 2; return 2; }
  virtual int write(unsigned char* buffer, const unsigned int length) { _l += length; return length;}
  unsigned int length() { return _l; }
  virtual void close() {}
};
