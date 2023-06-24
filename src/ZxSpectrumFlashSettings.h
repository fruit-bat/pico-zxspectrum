#include "ZxSpectrumSettings.h"

class ZxSpectrumFlashSettings : public ZxSpectrumSettings {
protected:
  virtual bool onSave(ZxSpectrumSettingValues *values);
  virtual bool onLoad(ZxSpectrumSettingValues *values);
public:
  ZxSpectrumFlashSettings();
};
