// SPDX-License-Identifier: MIT
// Copyright (c) 2026 fruit-bat
#pragma once

#include <pico/printf.h>

class ZxSpectrumKiosk {
public:
  virtual ~ZxSpectrumKiosk() {}
  virtual bool isKiosk() { return false; }
};
