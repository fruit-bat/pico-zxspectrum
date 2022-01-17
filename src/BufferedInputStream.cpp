#include "BufferedInputStream.h"


BufferedInputStream::BufferedInputStream(InputStream* delegate) :
  _delegate(delegate),
  _len(0),
  _pos(0)
{
}

void BufferedInputStream::flood() {
  _len = _delegate->read(_buffer, sizeof(_buffer));
  _pos = 0;
}

int BufferedInputStream::readByte() {
  if (_pos >= _len) {
    flood();
  }
  if (_len < 0) return _len;
  if (_len == 0) return -1;
  return _buffer[_pos++];
}

void BufferedInputStream::close() {
  _delegate->close();
}

bool BufferedInputStream::closed() {
  return _delegate->closed();
}

bool BufferedInputStream::end() {
  return _delegate->end();
}
