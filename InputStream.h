#pragma once

class InputStream {
  
public:
  virtual ~InputStream() { close(); }
  virtual int readByte(); // read a single byte, -1 for eof, -ve for error
  virtual int readWord(); // read 2 bytes LSB first, -1 for eof, -ve for error
  virtual int read(unsigned char* buffer, const unsigned int length); // returns actual length read, -1 for eof, -ve for error
  virtual void close() {}
  virtual bool closed();
  virtual bool end();
};
