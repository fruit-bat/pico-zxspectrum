#include "QuickSave.h"
#include <string.h>
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"

QuickSave::QuickSave(SdCardFatFsSpi* sdCard, const char* name) :
  _sdCard(sdCard),
  _name(name)
{
}

QuickSave::~QuickSave() {
}

void QuickSave::save(ZxSpectrum *specy, int slot) {
  char name[280];
  sprintf(name, "%s/slot%d.z80", _name.c_str(), slot);
  printf("Quick save file to save %s\n", name);
  FatFsSpiOutputStream os(_sdCard, name);
  specy->saveZ80(&os);
}

void QuickSave::load(ZxSpectrum *specy, int slot) {
  char name[280];
  sprintf(name, "%s/slot%d.z80", _name.c_str(), slot);
  printf("Quick save file to load %s\n", name);
  FatFsSpiInputStream is(_sdCard, name);
  specy->loadZ80(&is);
}

bool QuickSave::copy(int slot, const char *folder, const char *name) {
  char name1[280];
  sprintf(name1, "%s/slot%d.z80", _name.c_str(), slot);
  std::string name2(folder);
  name2.append("/");
  name2.append(name);
    // Should not be case sensitive
  auto nl = strlen(name);
  if (nl < 4 || !((strncmp(name + nl - 4, ".z80", 4) == 0) || (strncmp(name + nl - 4, ".Z80", 4) == 0))) {
    name2.append(".z80");
  }
  FatFsSpiOutputStream os(_sdCard, name2.c_str());
  FatFsSpiInputStream is(_sdCard, name1);
  if (os.closed() || is.closed()) return false;
  uint8_t buf[64];
  int l = 0;
  while(true) {
    l = is.read(buf, sizeof(buf));
    if (l < 0) break;
    os.write(buf, l);
  }
  return l == -1;
}

