#include "ZxSpectrumFileSettings.h"
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"

ZxSpectrumFileSettings::ZxSpectrumFileSettings(SdCardFatFsSpi* sdCard, const char *folder, const char *file) :
  ZxSpectrumSettings(),
  _sdCard(sdCard),
  _path(folder)
{
    _path.push(file);
}

bool ZxSpectrumFileSettings::onSave(ZxSpectrumSettingValues *values) {
  std::string name;
  _path.appendTo(name);
  FatFsSpiOutputStream os(_sdCard, name.c_str());
  if (os.closed()) return false;
  return os.write((uint8_t*)values, sizeof(values)) == sizeof(values);
}

bool ZxSpectrumFileSettings::onLoad(ZxSpectrumSettingValues *values) {
  std::string name;
  _path.appendTo(name);
  FatFsSpiInputStream is(_sdCard, name.c_str());
  if (is.closed()) return false;
  return is.read((uint8_t*)values, sizeof(values)) == sizeof(values);
}
