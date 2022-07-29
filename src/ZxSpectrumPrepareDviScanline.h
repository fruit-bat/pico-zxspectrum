#pragma once

#include "pico/stdlib.h"

extern "C" {
#include "dvi.h"
}

#ifdef __cplusplus
extern "C" {
#endif

void __not_in_flash_func(zx_prepare_hdmi_scanline)(
  struct dvi_inst *dvi, 
  uint32_t y, 
  uint32_t frame,
  uint8_t* screenPtr,
  uint8_t* attrPtr,
  uint8_t borderColor
);

#ifdef __cplusplus
}
#endif
