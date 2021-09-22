#include "ZxSpectrumArraySnapZ80.h"
#include "ArrayInputStream.h"

ZxSpectrumArraySnapZ80::ZxSpectrumArraySnapZ80(
  const unsigned char* buf,
  const unsigned int len) :
  _buf(buf),
  _len(len)
{
}

void ZxSpectrumArraySnapZ80::load(ZxSpectrum *specy) {
  ArrayInputStream is(_buf, _len);
  specy->loadZ80(&is);	
}
