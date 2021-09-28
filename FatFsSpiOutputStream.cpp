#include "FatFsSpiOutputStream.h"
#include <pico/printf.h>

FatFsSpiOutputStream::FatFsSpiOutputStream(SdCardFatFsSpi* sdCard, const char* name) :
  _sdCard(sdCard),
  _open(false)
{
  if (!_sdCard->mounted()) {
    if (!_sdCard->mount()) {
      printf("Failed to mount SD card\n");
      return;
    }
  }
  
  printf("openning file %s for write \n", name);
  _fr = f_open(&_fil, name, FA_WRITE|FA_OPEN_ALWAYS);
  if (FR_OK != _fr && FR_EXIST != _fr) {
    printf("f_open(%s) error: %s (%d)\n", name, FRESULT_str(_fr), _fr);
  }
  else {
    printf("openned file %s ok!\n", name);
    _open = true;
    _fr = FR_OK;
  }
}

int FatFsSpiOutputStream::write(unsigned char* buffer, const unsigned int length) {
  if (!_open) return -1;
  
  // TODO Handle errors with separate codes
  if (FR_OK != _fr) return -1;

  UINT bw = 0;
  _fr = f_write(&_fil, buffer, length, &bw);
  if (_fr != FR_OK) {
    printf("f_write(%s) error: (%d)\n", FRESULT_str(_fr), _fr);
    return -2;
  }
  if (bw < length) {
    printf("f_write(%s) error: wrote %d of %d\n", bw, length);
    return -3; // failed to write
  }
  return bw;
}

void FatFsSpiOutputStream::close() {
  if (_open) {
    f_close(&_fil);
    _open = false;
  }
}
