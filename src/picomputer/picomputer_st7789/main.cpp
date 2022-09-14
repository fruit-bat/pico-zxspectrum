#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"

#include "pzx_prepare_rgb444_scanline.h"
#include "PicoCharRendererSt7789.h"
#include "st7789_lcd.h"

#include "pzx_keyscan.h"

#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "PicoPen.h"
#include "PicoTextField.h"
#include "PicoWinHidKeyboard.h"

#include "ZxSpectrumFatSpiKiosk.h"
#include "ZxSpectrum.h"
#include "ZxSpectrumHidKeyboard.h"
#include "ZxSpectrumDualJoystick.h"
#include "ZxSpectrumHidJoystick.h"
#include "ZxSpectrumPicomputerJoystick.h"

#include "bsp/board.h"
#include "tusb.h"
#include <pico/printf.h>
#include "SdCardFatFsSpi.h"
#include "QuickSave.h"
#include "ZxSpectrumFatFsSpiFileLoop.h"

#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "ZxSpectrumMenu.h"
#include "ZxSpectrumAudio.h"

#define LED_PIN 25

#define VREG_VSEL VREG_VOLTAGE_1_10

struct semaphore dvi_start_sem;

uint8_t* screenPtr;
uint8_t* attrPtr;

static SdCardFatFsSpi sdCard0(0);

// ZX Spectrum emulator
static ZxSpectrumFatFsSpiFileLoop zxSpectrumSnaps(
  &sdCard0,
  "zxspectrum/snapshots"
);
static ZxSpectrumFatFsSpiFileLoop zxSpectrumTapes(
  &sdCard0,
  "zxspectrum/tapes"
);
static QuickSave quickSave(
  &sdCard0,
  "zxspectrum/quicksaves"
);
static ZxSpectrumHidJoystick hidJoystick;
static ZxSpectrumPicomputerJoystick picomputerJoystick;
static ZxSpectrumDualJoystick dualJoystick(
  &hidJoystick, 
  &picomputerJoystick
);
static ZxSpectrumFatSpiKiosk zxSpectrumKisok(
  &sdCard0,
  "zxspectrum"
);
static ZxSpectrumHidKeyboard keyboard1(
  &zxSpectrumSnaps, 
  &zxSpectrumTapes, 
  &quickSave, 
  &dualJoystick
);
static ZxSpectrumHidKeyboard keyboard2(
  &zxSpectrumSnaps, 
  &zxSpectrumTapes,
  &quickSave, 
  &picomputerJoystick
);
static ZxSpectrum zxSpectrum(
  &keyboard1, 
  &keyboard2, 
  &dualJoystick
);
static ZxSpectrumMenu picoRootWin(
  &sdCard0, 
  &zxSpectrum, 
  &quickSave
);
static PicoDisplay picoDisplay(
  pcw_screen(), 
  &picoRootWin
);
static PicoWinHidKeyboard picoWinHidKeyboard(
  &picoDisplay
);

static bool showMenu = true;
static bool toggleMenu = false;
static volatile uint _frames = 0;

extern "C"  void __not_in_flash_func(process_kbd_mount)(uint8_t dev_addr, uint8_t instance) {
	keyboard1.mount();
}

extern "C"  void __not_in_flash_func(process_kbd_unmount)(uint8_t dev_addr, uint8_t instance) {
	keyboard1.unmount();
}

extern "C"  void __not_in_flash_func(process_kbd_report)(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report) {
  int r;
  if (showMenu) {
    r = picoWinHidKeyboard.processHidReport(report, prev_report);
  }
  else {
    r = keyboard1.processHidReport(report, prev_report);
  }
  if (r == 1) {
    toggleMenu = true;
    picoRootWin.repaint();
  }
}

void __not_in_flash_func(process_picomputer_kbd_report)(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report) {
  int r;
  if (showMenu) {
    r = picoWinHidKeyboard.processHidReport(report, prev_report);
  }
  else {
    r = keyboard2.processHidReport(report, prev_report);
  }
  if (r == 1) {
    toggleMenu = true;
    picoRootWin.repaint();
  }
}


static  PIO pio = pio0;
static  uint sm = 0;

void __not_in_flash_func(core1_main)() {
  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

  uint32_t t1 = time_us_32();

  while (1) {

    for (uint y = 0; y < 240; ++y) {
      if (showMenu) {
        pcw_send_st7789_scanline(
          pio, 
          sm,
          y,
          _frames);
      }
      else {  
        pzx_send_rgb444_scanline(
          pio, 
          sm,
          y,
          _frames,
          screenPtr,
          attrPtr,
          zxSpectrum.borderColour()); 
      }

      pzx_keyscan_row();
    }
    _frames++;

    // TODO Tidy this mechanism up
    screenPtr = zxSpectrum.screenPtr();
    attrPtr = screenPtr + (32 * 24 * 8);
    
    if (toggleMenu) {
      showMenu = !showMenu;
      toggleMenu = false;
      picomputerJoystick.enabled(!showMenu);
    }
   
    while((time_us_32() - t1) < 20000) {
      sleep_us(100);
      pzx_keyscan_row();
    }
    
    t1 += 20000;
  }
  __builtin_unreachable();
}

void __not_in_flash_func(main_loop)() {

  unsigned int lastInterruptFrame = _frames;
    
  //Main Loop 
  uint frames = 0;
  uint c = 0;
  
  while(1){
    
    if (c++ & 1) {
      tuh_task();
    }
    else {
      hid_keyboard_report_t const *curr;
      hid_keyboard_report_t const *prev;
      pzx_keyscan_get_hid_reports(&curr, &prev);
      process_picomputer_kbd_report(curr, prev);
    }
    if (!showMenu) {
      for (int i = 1; i < 100; ++i) {
        if (lastInterruptFrame != _frames) {
          lastInterruptFrame = _frames;
          zxSpectrum.interrupt();
        }
        zxSpectrum.step();
      }
    }
    else if (frames != _frames) {
      frames = _frames;
      picoDisplay.refresh();
    }
  }
}

int main() {
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);
  set_sys_clock_khz(200000, true);

  //Initialise I/O
  stdio_init_all(); 
  
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  tusb_init();

  // Configure the GPIO pins for audio
  zxSpectrumAudioInit();

  screenPtr = zxSpectrum.screenPtr();
  attrPtr = screenPtr + (32 * 24 * 8);

  keyboard1.setZxSpectrum(&zxSpectrum);
  keyboard2.setZxSpectrum(&zxSpectrum);
	 
  // Initialise the menu renderer
  pcw_init_renderer();

  // Initialise the keyboard scan
  pzx_keyscan_init();

  // Start up the LCD
  st7789_init(pio, sm);
  
  sleep_ms(10);
   
  sem_init(&dvi_start_sem, 0, 1);
  
  multicore_launch_core1(core1_main);
  
  picoRootWin.showMessage([=](PicoPen *pen) {
    pen->printAtF(3, 1, false, "Reading from SD card...");
  });
          
  picoDisplay.refresh();
  
  sem_release(&dvi_start_sem);

  if (sdCard0.mount()) {
		
		// Set up the quick load loops
		zxSpectrumSnaps.reload();
		zxSpectrumTapes.reload();

    // Load quick save slot 1 if present
		if (quickSave.used(0)) {
			quickSave.load(&zxSpectrum, 0);
		}
  
    // See if the board is in kiosk mode    
    bool isKiosk = zxSpectrumKisok.isKiosk();
    keyboard1.setKiosk(isKiosk);
    keyboard2.setKiosk(isKiosk);
	}
  
  showMenu = false;
  picoRootWin.removeMessage();
  
  main_loop();
}
