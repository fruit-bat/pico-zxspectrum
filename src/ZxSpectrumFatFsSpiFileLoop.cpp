#include "ZxSpectrumFatFsSpiFileLoop.h"
#include <pico/printf.h>
#include "FatFsSpiInputStream.h"
#include "BufferedInputStream.h"
#include <string.h>

ZxSpectrumFatFsSpiFileLoop::ZxSpectrumFatFsSpiFileLoop(SdCardFatFsSpi* sdCard, const char *folder) :
  _sdCard(sdCard),
  _folder(folder),
  _is(0)
{
  reload();
}

ZxSpectrumFatFsSpiFileLoop::~ZxSpectrumFatFsSpiFileLoop() {
  // TODO free any memory

  if (_is) {
    _is->close();
    delete _is;
  }
}

void ZxSpectrumFatFsSpiFileLoop::reload() {
  if (!_sdCard->mounted()) {

    // TODO clear down existing list
    
    if (!_sdCard->mount()) {
      printf("Failed to mount SD card\n");
      return;
    }
  }
  
  DIR dj;         /* Directory object */
  FILINFO fno;    /* File information */
  FRESULT dfr = f_findfirst(&dj, &fno, _folder, "*.*");

  while (dfr == FR_OK && fno.fname[0]) {
    printf("file %s\n", fno.fname);
    ZxSpectrumFile *fsi = new ZxSpectrumFile(fno.fname);
    add(fsi);
    dfr = f_findnext(&dj, &fno); // Search for next item
  }
}

const char *ZxSpectrumFatFsSpiFileLoop::fext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

void ZxSpectrumFatFsSpiFileLoop::load(ZxSpectrum* zxSpectrum) {
  if (!_sdCard->mounted()) { 
    if (!_sdCard->mount()) {
      printf("Failed to mount SD card\n");
      return;
    }
  }
  char name[280];
  sprintf(name, "%s/%s", _folder, curr()->name());
  printf("File to load %s\n", name);
  
  if (_is) {
    zxSpectrum->loadTap(0);
    delete _is;
  }
  FatFsSpiInputStream *is = new FatFsSpiInputStream(_sdCard, name);
 
  const char *ext = fext(curr()->name());
  if (0 == strcmp(ext, "z80")) {
    zxSpectrum->loadZ80(is);
    delete is;
  }
  else if (0 == strcmp(ext, "tap")) {
    _is = is;
    zxSpectrum->loadTap(_is);
  }
  else {
    delete is;
  }
}
