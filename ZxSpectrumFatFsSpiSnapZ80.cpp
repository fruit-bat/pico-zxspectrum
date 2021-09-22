#include "ZxSpectrumFatFsSpiSnapZ80.h"
#include "FatFsSpiInputStream.h"
#include <string.h>

ZxSpectrumFatFsSpiSnapZ80::ZxSpectrumFatFsSpiSnapZ80(
  SdCardFatFsSpi* sdCard, 
  const char* name) :
  _sdCard(sdCard)
{
  const int l = strlen(name);
  _name = new char[l+1]();
  strcpy(_name, name);
}

void ZxSpectrumFatFsSpiSnapZ80::load(ZxSpectrum *specy) {
  FatFsSpiInputStream is(_sdCard, _name);
  specy->loadZ80(&is);	
}

ZxSpectrumFatFsSpiSnapZ80::~ZxSpectrumFatFsSpiSnapZ80() {
  delete _name;
}
