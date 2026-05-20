// SPDX-License-Identifier: MIT
// Copyright (c) 2026 fruit-bat
#pragma once

#include "InputStream.h"

class ZxSpectrumFile {
  ZxSpectrumFile *_p;
  ZxSpectrumFile *_n;
  char* _name;
public:
  ZxSpectrumFile(char* name);
  ~ZxSpectrumFile();
  void link(ZxSpectrumFile *c);
  ZxSpectrumFile* prev() { return _p; }
  ZxSpectrumFile* next() { return _n; }
  char *name() { return _name; }
};
