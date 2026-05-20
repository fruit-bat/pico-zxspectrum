// SPDX-License-Identifier: MIT
// Copyright (c) 2026 fruit-bat
#pragma once

#include "ZxSpectrumSettings.h"

class ZxSpectrumFlashSettings : public ZxSpectrumSettings {
protected:
  bool onSave(ZxSpectrumSettingValues *values);
  bool onLoad(ZxSpectrumSettingValues *values);
public:
  ZxSpectrumFlashSettings();
};
