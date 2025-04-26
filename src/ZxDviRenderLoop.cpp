#include "ZxDviRenderLoop.h"
#include "ZxSpectrumPrepareDviScanline.h"
#include "hardware/clocks.h"
#include "PicoCharRenderer.h"

extern "C" {
    #include "dvi.h"
    #include "dvi_serialiser.h"
    #ifdef DVI_CUSTOM_CONFIG
    #include "hdmi/custom_dvi_pin_configs.h"
    #else
    #include "common_dvi_pin_configs.h"
    #endif
    #include "tmds_encode_zxspectrum.h"
}

struct dvi_inst dvi0;
bool dvi_running = false;

zx_spectrum_audio_driver_enum_t ZxDviRenderLoopAudioDefault() {
#if defined(PICO_DEFAULT_AUDIO)
  return PICO_DEFAULT_AUDIO;
#elif defined(PICO_DEFAULT_AUDIO_DVI)
  return PICO_DEFAULT_AUDIO_DVI;
#elif defined(PICO_HDMI_AUDIO)
  return zx_spectrum_audio_driver_hdmi_index;
#elif defined(PICO_AUDIO_I2S)
  return zx_spectrum_audio_driver_i2s_index;
#elif defined(PICO_PIO_PWM_AUDIO)
  return zx_spectrum_audio_driver_pio_pwm_index;
#elif defined(PICO_PWM_AUDIO)
  return zx_spectrum_audio_driver_pwm_index;
#else
  return zx_spectrum_audio_driver_pwm_index;
#endif 
}

void ZxDviRenderLoopInit_s(const struct dvi_timing *t) {
  // Run system at TMDS bit clock
  set_sys_clock_khz(t->bit_clk_khz, true);
  sleep_ms(10);

  dvi0.timing = t;
  dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
  dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());
}

void ZxDviRenderLoopInit() {
  ZxDviRenderLoopInit_s(&DVI_TIMING);
}

void __not_in_flash_func(ZxDviRenderLoop)(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    volatile bool &toggleMenu,
    ZxSpectrumMenu& picoRootWin)
{
    dvi_register_irqs_this_core(&dvi0, DMA_IRQ_1);
    dvi_start(&dvi0);
    dvi_running = true;
    
    uint8_t* screenPtr;
    uint8_t* attrPtr;
    screenPtr = zxSpectrum.screenPtr();
    attrPtr = screenPtr + (32 * 24 * 8);

    uint y = 0;
    uint ys = 0;
    for(int i = 0; i < (DISPLAY_BLANK_LINES/2); ++i) {
      if (showMenu) {
        pcw_prepare_blankline_80(&dvi0, frames);
      }
      else {
        zx_prepare_hdmi_scanline(&dvi0, y, frames, screenPtr, attrPtr, 0);
      }
    }
    while(true) {
      const uint32_t fpf = zxSpectrum.flipsPerFrame();
      const bool ringoMode = fpf > 46 && fpf < 52;
      if (ringoMode) {
        screenPtr = zxSpectrum.memPtr(y & 4 ? 7 : 5);
        attrPtr = screenPtr + (32 * 24 * 8);
      }
  
      if (showMenu) {
        uint rs = pcw_prepare_scanline_80(&dvi0, y++, ys, frames);
        if (0 == (y & 7)) {
          ys += rs;
        }
      }
      else {
        zx_prepare_hdmi_scanline(
          &dvi0, 
          y, 
          frames, 
          screenPtr, 
          attrPtr, 
          zxSpectrum.borderColour(y)
        );
        y++;
      }
      ZxRenderLoopCallbackLine(y);
      if (y == ZX_SPECTRUM_SCREEN_HEIGHT) {
        y = 0;
        ys = 0;
        for(int i = 0; i < DISPLAY_BLANK_LINES; ++i) {
          if (showMenu) {
            pcw_prepare_blankline_80(&dvi0, frames);
          }
          else {           
            zx_prepare_hdmi_scanline(&dvi0, 239, frames, screenPtr, attrPtr, 0);
          }
        }
       frames++;
        if (!ringoMode) {
          screenPtr = zxSpectrum.screenPtr();
          attrPtr = screenPtr + (32 * 24 * 8);
        }
        if (toggleMenu) {
          showMenu = !showMenu;
          toggleMenu = false;
          ZxRenderLoopCallbackMenu(showMenu);
        }
      }
    }
}
