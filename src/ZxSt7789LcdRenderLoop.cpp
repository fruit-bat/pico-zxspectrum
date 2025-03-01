#include "ZxSt7789LcdRenderLoop.h"
#include "picomputer/picomputer_st7789/st7789_lcd.h"
#include "picomputer/picomputer_st7789/pzx_prepare_rgb444_scanline.h"
#include "pico/stdlib.h"
#include "PicoCharRendererSt7789.h"
#include "PicoCharRendererVga.h"

static  PIO pio = pio0;
static  uint sm = 0;

void ZxSt7789LcdRenderLoopInit(ZxSpectrumMenu& picoRootWin) {

  // Turn on the LCD backlight
//   gpio_init(4);
//   gpio_set_dir(4, GPIO_OUT);
//   gpio_put(4, 1);

   picoRootWin.move(0,0,40,30);
   picoRootWin.setWizLayout(0, 12, 18, 40);

  // Start up the LCD
  st7789_init(pio, sm);

  sleep_ms(10);
}

void ZxSt7789LcdRenderLoop(
    ZxSpectrum &zxSpectrum,
    volatile uint &frames,
    bool &showMenu,
    bool &toggleMenu) 
{
  uint32_t t1 = time_us_32();
  uint8_t* screenPtr;
  uint8_t* attrPtr;
  screenPtr = zxSpectrum.screenPtr();
  attrPtr = screenPtr + (32 * 24 * 8);

  while (1) {

    for (uint y = 0; y < 240; ++y) {

      const uint32_t fpf = zxSpectrum.flipsPerFrame();
      const bool ringoMode = fpf > 46 && fpf < 52;
      if (ringoMode) {
        screenPtr = zxSpectrum.memPtr(y & 4 ? 7 : 5);
        attrPtr = screenPtr + (32 * 24 * 8);
      }

      if (y == 0) {

        if (!ringoMode) {
          screenPtr = zxSpectrum.screenPtr();
          attrPtr = screenPtr + (32 * 24 * 8);
        }

        frames++;
      }

      if (showMenu) {
        pcw_send_st7789_scanline(
          pio,
          sm,
          y,
          frames);
      }
      else {
        pzx_send_rgb444_scanline(
          pio,
          sm,
          y,
          frames,
          screenPtr,
          attrPtr,
          zxSpectrum.borderColour(y));
      }

      ZxSt7789LcdRenderLoopCallbackLine(y);
    }

    if (toggleMenu) {
      showMenu = !showMenu;
      toggleMenu = false;
      ZxSt7789LcdRenderLoopCallbackMenu(showMenu);
    }

    while((time_us_32() - t1) < 20000) {
      sleep_us(100);
      ZxSt7789LcdRenderLoopCallbackLine(-1);
    }

    t1 += 20000;
  }
  __builtin_unreachable();
}
