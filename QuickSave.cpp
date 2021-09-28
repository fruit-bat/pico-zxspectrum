#include "QuickSave.h"
#include <string.h>
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"

QuickSave::QuickSave(SdCardFatFsSpi* sdCard, const char* name) :
  _sdCard(sdCard)
{
  const int l = strlen(name);
  _name = new char[l+1]();
  strcpy(_name, name);
}

QuickSave::~QuickSave() {
  delete _name;
}

void QuickSave::save(ZxSpectrum *specy, int slot) {
  char name[280];
  sprintf(name, "%s/slot%d.z80", _name, slot);
  printf("Quick save file to save %s\n", name);
  FatFsSpiOutputStream os = FatFsSpiOutputStream(_sdCard, name);
  specy->saveZ80(&os);
}

void QuickSave::load(ZxSpectrum *specy, int slot) {
  char name[280];
  sprintf(name, "%s/slot%d.z80", _name, slot);
  printf("Quick save file to load %s\n", name);
  FatFsSpiInputStream is(_sdCard, name);
  specy->loadZ80(&is);	
}
