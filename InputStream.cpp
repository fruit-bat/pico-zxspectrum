#include "InputStream.h"

int InputStream::readWord() {
  unsigned char buf[2];
  const int r = read(buf, 2);
  if (r < 0) return r;
  return buf[0] + (((int)buf[1]) << 8);
}

int InputStream::read(unsigned char* buffer, const unsigned int length) {
  unsigned int i = 0;
  while (true) {
    if (i == length) return i;
    const int r = readByte();
    if (r == -1) return i;
    if (r < 0) return r;
    buffer[i++] = r;
  }
}
