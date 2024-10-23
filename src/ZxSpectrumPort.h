#pragma once
#include <pico/stdlib.h>

class ZxSpectrumPort {
public:
  virtual uint8_t __not_in_flash_func(read)() = 0;
  virtual void __not_in_flash_func(write)(uint8_t data) = 0;
};
