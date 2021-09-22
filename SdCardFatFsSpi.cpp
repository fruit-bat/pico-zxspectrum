#include <pico/printf.h>

#include "SdCardFatFsSpi.h"
#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"
#include "rtc.h"
//
#include "hw_config.h"

SdCardFatFsSpi::SdCardFatFsSpi(int unit) :
  _unit(unit),
  _sdcard(0)
{
}

bool SdCardFatFsSpi::mount() {
  static bool timeInitialised = false;
  if (mounted()) return true;
  if (!timeInitialised) {
    printf("Initialising time for SD card %d... ", _unit);
    time_init();
    printf("Ok\n");
    timeInitialised = true;
  }
  _sdcard = sd_get_by_num(_unit);
  printf("Mounting SD card %d... ", _unit);
  FRESULT fr = f_mount(&_sdcard->fatfs, _sdcard->pcName, 1);
  if (FR_OK != fr) {
    printf(" error: %s (%d)\n", FRESULT_str(fr), fr);
    _sdcard = 0;
  }
  else {
    printf("Ok\n");
  }
  return mounted();
}

void SdCardFatFsSpi::unmount() {
  if (mounted()) {
    printf("Unmounting SD card %d... ", _unit);
    f_unmount(_sdcard->pcName);
    printf("Ok\n");
    _sdcard = 0;
  }
}
