#include "ZxSpectrumDirToSnap.h"
#include "ZxSpectrumFatFsSpiSnapZ80.h"
#include <pico/printf.h>

ZxSpectrumDirToSnap::ZxSpectrumDirToSnap(SdCardFatFsSpi* sdCard) :
  _sdCard(sdCard) 
{
}

void ZxSpectrumDirToSnap::addToList(const char *folderName, ZxSpectrumSnapList *list) {
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) {
      printf("Failed to mount SD card\n");
      return;
    }
  }
  
  DIR dj;         /* Directory object */
  FILINFO fno;    /* File information */
  FRESULT dfr = f_findfirst(&dj, &fno, folderName, "*.z80");

  while (dfr == FR_OK && fno.fname[0]) {
    char name[280];
    sprintf(name, "%s/%s", folderName, fno.fname);
    printf("Snap file %s\n", name);
    ZxSpectrumFatFsSpiSnapZ80 *fsi = new ZxSpectrumFatFsSpiSnapZ80(_sdCard, name);
    list->add(fsi);
    dfr = f_findnext(&dj, &fno); // Search for next item
  }
}
