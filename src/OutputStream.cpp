#include "OutputStream.h"

int OutputStream::writeByte(int b) {
  unsigned char buf[1];
  buf[0] = b;
  return write((unsigned char *)&buf, 1);
}

int OutputStream::writeWord(int w) {
  unsigned char buf[2];
  buf[0] = w & 0xff;
  buf[1] = w >> 8;
  return write((unsigned char *)&buf, 2);
}
