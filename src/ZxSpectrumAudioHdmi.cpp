#include "ZxSpectrumAudioHdmi.h"

#if !defined(PICO_HDMI_AUDIO)
#include "ZxSpectrumAudioNull.h"

void hdmi_audio_init()
{
  null_audio_init();
}

uint32_t hdmi_audio_freq()
{
  return null_audio_freq();
}

void __not_in_flash_func(hdmi_audio_handler)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute)
{
  null_audio_handler(vA, vB, vC, s, buzzer, mute);
}

bool __not_in_flash_func(hdmi_audio_ready)()
{
  return null_audio_ready();
}

#else

#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ZxSpectrumAudioVol.h"
#include "ZxSpectrumAudio16BitStereo.h"

#include "dvi.h"

extern struct dvi_inst dvi0;

#define AUDIO_BUFFER_SIZE   256
audio_sample_t      audio_buffer[AUDIO_BUFFER_SIZE];

#ifndef PICO_HDMI_AUDIO_FREQ
#define PICO_HDMI_AUDIO_FREQ 44100
#endif

#if (PICO_HDMI_AUDIO_FREQ == 32000)
#define HDMI_N     4096     // From HDMI standard for 32kHz
#elif (PICO_HDMI_AUDIO_FREQ == 44100)
#define HDMI_N     6272     // From HDMI standard for 44.1kHz
#else
#define HDMI_N     6144     // From HDMI standard for 48kHz
#endif

void hdmi_audio_init()
{
  dvi_get_blank_settings(&dvi0)->top    = 0;
  dvi_get_blank_settings(&dvi0)->bottom = 0;
  dvi_audio_sample_buffer_set(&dvi0, audio_buffer, AUDIO_BUFFER_SIZE);
  dvi_set_audio_freq(
    &dvi0, 
    PICO_HDMI_AUDIO_FREQ, 
    dvi0.timing->bit_clk_khz*HDMI_N/(PICO_HDMI_AUDIO_FREQ/100)/128,
    HDMI_N
  );
  increase_write_pointer(&dvi0.audio_ring, get_write_size(&dvi0.audio_ring, true));
}

uint32_t hdmi_audio_freq() {
  return PICO_HDMI_AUDIO_FREQ;
}

void __not_in_flash_func(hdmi_audio_handler)(uint32_t vA, uint32_t vB, uint32_t vC, int32_t s, uint32_t buzzer, bool mute)
{
  uint32_t ll, rr;
  audio_handler_16bit_helper(vA, vB, vC, s, buzzer, mute, ll, rr);
  audio_sample_t *audio_ptr = get_write_pointer(&dvi0.audio_ring);
	audio_ptr->channels[0] = ll;
	audio_ptr->channels[1] = rr;
	increase_write_pointer(&dvi0.audio_ring, 1);
}

bool __not_in_flash_func(hdmi_audio_ready)()
{
  return get_write_size(&dvi0.audio_ring, true) > 0;
}

#endif