#include "ZxSpectrumSettings.h"

class ZxSpectrumFlashSettings : public ZxSpectrumSettings {
protected:
  bool onSave(ZxSpectrumSettingValues *values);
  bool onLoad(ZxSpectrumSettingValues *values);
public:
  ZxSpectrumFlashSettings();
};
