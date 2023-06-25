#include "ZxSpectrumSettings.h"
#include "SdCardFatFsSpi.h"
#include "FatFsFilePath.h"


class ZxSpectrumFileSettings : public ZxSpectrumSettings {
private:
  SdCardFatFsSpi* _sdCard;
  FatFsFilePath _path;
protected:
  bool onSave(ZxSpectrumSettingValues *values);
  bool onLoad(ZxSpectrumSettingValues *values);
public:
  ZxSpectrumFileSettings(SdCardFatFsSpi* sdCard, const char *folder, const char *file);
};
