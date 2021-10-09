#pragma once
#include "InputStream.h"

class ArrayInputStream : public InputStream {
  const unsigned char* _buf;
  unsigned int _length;
  unsigned int _i;
public:
  ArrayInputStream(
    const unsigned char* buf,
    unsigned int length
  ) :
    _buf(buf),
    _length(length),
    _i(0)
  {}
  
  virtual int readByte() { 
    return _i >= _length ? -1 : _buf[_i++];
  }
  
  virtual void close() {}

  virtual bool closed() { return false; }
  virtual bool end() { return _i >= _length; }
};
