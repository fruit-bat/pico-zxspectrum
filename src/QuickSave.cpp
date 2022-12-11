#include "QuickSave.h"
#include <string.h>
#include "FatFsSpiOutputStream.h"
#include "FatFsSpiInputStream.h"

QuickSave::QuickSave(SdCardFatFsSpi* sdCard, const char* folder) :
  _sdCard(sdCard),
  _folder(folder)
{
}

QuickSave::~QuickSave() {
}
  
void QuickSave::filename(const char *buf, int slot) {
  snprintf((char *)buf, 280, "%s/slot%d.z80", _folder.c_str(), slot);
}

void QuickSave::save(ZxSpectrum *specy, int slot) {
  char name[280]; filename(name, slot);
  printf("Quick save file to save %s\n", name);
  FatFsSpiOutputStream os(_sdCard, name);
  specy->saveZ80(&os);
}

void QuickSave::load(ZxSpectrum *specy, int slot) {
  char name[280]; filename(name, slot);
  printf("Quick save file to load %s\n", name);
  FatFsSpiInputStream is(_sdCard, name);
  specy->loadZ80(&is);
  sprintf(name, "Quick Save %d", slot + 1); 
  if (_listener) _listener(slot, name);
}

bool QuickSave::copy(int slot, const char *fname) {
  char name1[280]; filename(name1, slot);
  FatFsSpiOutputStream os(_sdCard, fname);
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

bool QuickSave::used(int slot) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  char name[280]; filename(name, slot);
  FILINFO fno;
  FRESULT fr = f_stat(name, &fno);
  return fr == FR_OK;
}

bool QuickSave::clear(int slot) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) return false;
  }
  char name[280]; filename(name, slot);
  return f_unlink(name) == FR_OK;
}
