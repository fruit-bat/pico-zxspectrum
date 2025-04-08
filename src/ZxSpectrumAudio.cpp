#include "ZxSpectrumAudio.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "pico/printf.h"

#include "ZxSpectrumAudioPioPwm.h"

#ifdef EAR_PIN
#include "zx_ear_in.pio.h"

static  PIO ear_pio = pio1;
static  uint ear_sm = 0;

static void init_ear_in() {
  uint offset = pio_add_program(ear_pio, &zx_ear_in_program);
  ear_sm = pio_claim_unused_sm(ear_pio, true);
  zx_ear_in_program_init(ear_pio, ear_sm, offset, EAR_PIN, 1000000);
}
#endif

uint32_t __not_in_flash_func(zxSpectrumReadEar)() {
#ifdef EAR_PIN
    return ear_pio->rxf[ear_sm];
#else
    return 0;
#endif
}

bool __not_in_flash_func(zxSpectrumEarReady)() {
#ifdef EAR_PIN
    return (ear_pio->fstat & (1u << (PIO_FSTAT_RXEMPTY_LSB + ear_sm))) == 0;
#else
    return true;
#endif
}

zx_spectrum_audio_driver_t* zxSpectrumAudioInit(zx_spectrum_audio_driver_enum_t audio_driver_index) {
  zx_spectrum_audio_driver_init();
#ifdef EAR_PIN
  init_ear_in();
#endif
  return &_zx_spectrum_audio_drivers[audio_driver_index];
}
