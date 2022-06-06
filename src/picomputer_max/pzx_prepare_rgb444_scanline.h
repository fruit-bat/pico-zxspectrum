#pragma once

#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

void __not_in_flash_func(pzx_prepare_rgb444_scanline)(
  uint32_t* buf, 
  uint32_t y, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
);

void __not_in_flash_func(pzx_send_rgb444_scanline)(
  uint32_t y, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
);

#ifdef __cplusplus
}
#endif
