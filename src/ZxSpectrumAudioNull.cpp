
#include "ZxSpectrumAudioNull.h"

uint32_t null_audio_init()
{
  return 28000;
}

void __not_in_flash_func(null_audio_handler)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute)
{
}

bool __not_in_flash_func(null_audio_ready)()
{
  return true;
}
