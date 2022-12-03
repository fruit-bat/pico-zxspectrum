#include "ZxSpectrumFatFsCacheFileLoop.h"
#include <pico/printf.h>
#include "FatFsSpiInputStream.h"
#include <string.h>

#define DEBUG_FAT_SPI

#ifdef DEBUG_FAT_SPI
#define DBG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif

ZxSpectrumFatFsCacheFileLoop::ZxSpectrumFatFsCacheFileLoop(SdCardFatFsSpi* sdCard, FatFsDirCache* cache) :
  _sdCard(sdCard),
  _cache(cache),
  _i(0)
{
  cache->filter([](const char* fname) -> bool {
    const char *ext = fext(fname);
    return (0 == strcmp(ext, "z80") || 0 == strcmp(ext, "Z80"));
  });
}

const char *ZxSpectrumFatFsCacheFileLoop::fext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

ZxSpectrumFatFsCacheFileLoop::~ZxSpectrumFatFsCacheFileLoop() {
}

void ZxSpectrumFatFsCacheFileLoop::next(ZxSpectrum* zxSpectrum) {
  if (++_i >= _cache->size()) {
    _i = 0;
  }
  load(zxSpectrum);
}

void ZxSpectrumFatFsCacheFileLoop::prev(ZxSpectrum* zxSpectrum) {
  if (_i-- == 0) {
    _i = _cache->size() - 1;
  }
  load(zxSpectrum);
}

void ZxSpectrumFatFsCacheFileLoop::curr(ZxSpectrum* zxSpectrum) {
  load(zxSpectrum);
}

void ZxSpectrumFatFsCacheFileLoop::load(ZxSpectrum* zxSpectrum) {
  
  if (_cache->size() == 0) return;
   
  if (_cache->pos() != _i) {
    _cache->seek(_i);
  }
  
  FILINFO info;
  if (_cache->read(&info)) {
    
    std::string name(_cache->folder());
    name.append("/");
    name.append(info.fname);
    DBG_PRINTF("File to load %s\n", name.c_str());
    
    FatFsSpiInputStream *is = new FatFsSpiInputStream(_sdCard, name.c_str());
   
    zxSpectrum->loadZ80(is);
    delete is;
  }
}

void ZxSpectrumFatFsCacheFileLoop::reload() {
  _cache->remove();
  _cache->create();
  _cache->open();
  _i = 0;
}

