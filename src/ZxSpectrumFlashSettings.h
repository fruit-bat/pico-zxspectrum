#include "ZxSpectrumSettings.h"

class ZxSpectrumFlashSettings : public ZxSpectrumSettings {
protected:
  virtual bool __not_in_flash_func(onSave)(ZxSpectrumSettingValues *values);
  virtual bool onLoad(ZxSpectrumSettingValues *values);
public:
  ZxSpectrumFlashSettings();
};
