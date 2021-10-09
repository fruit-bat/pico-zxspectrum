#include "ZxSpectrumFile.h"
#include <string.h>

ZxSpectrumFile::ZxSpectrumFile(char* name) :
 _p(this),
 _n(this)
{
  const int l = strlen(name);
  _name = new char[l+1]();
  strcpy(_name, name);
}

ZxSpectrumFile::~ZxSpectrumFile() {
  delete _name;
}

void ZxSpectrumFile::link(ZxSpectrumFile *c) {
    // Link this after c
    _n = c->_n;
    _n->_p = this;
    c->_n = this;
    _p = c;
}
