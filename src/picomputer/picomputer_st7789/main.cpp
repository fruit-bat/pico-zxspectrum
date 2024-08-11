#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"
#include "ZxSpectrumPrepareRgbScanline.h"
#include "pzx_prepare_rgb444_scanline.h"
#include "PicoCharRendererSt7789.h"
#include "PicoCharRendererVga.h"

#include "st7789_lcd.h"

#ifdef PICOMPUTER_PICOZX_LCD
#include "ZxSpectrumPrepareRgbScanline.h"
#include "ZxSpectrumFatSpiKiosk.h"
#include "ZxRgb332RenderLoop.h"
#endif
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
#include "ZxSpectrumHidMouse.h"

#include "bsp/board.h"
#include "tusb.h"
#include <pico/printf.h>
#include "SdCardFatFsSpi.h"
#include "QuickSave.h"
#include "ZxSpectrumFileLoop.h"
#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "ZxSpectrumMenu.h"
#include "ZxSpectrumAudio.h"
#include "FatFsDirCache.h"
#include "ZxSpectrumFileSettings.h"

#define LED_PIN 25

#define VREG_VSEL VREG_VOLTAGE_1_20

struct semaphore dvi_start_sem;

#ifdef PICOMPUTER_PICOZX_LCD
static bool useVga = false;
#endif

static SdCardFatFsSpi sdCard0(0);

// ZX Spectrum emulator
static ZxSpectrumFileLoop snapFileLoop;
static QuickSave quickSave;
static ZxSpectrumHidMouse mouse;
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
  &snapFileLoop, 
  &quickSave, 
  &dualJoystick,
  &mouse
);
static ZxSpectrumHidKeyboard keyboard2(
  &snapFileLoop, 
  &quickSave, 
  &picomputerJoystick,
  0
);
static ZxSpectrum zxSpectrum(
  &keyboard1, 
  &keyboard2, 
  &dualJoystick,
  &mouse
);
static ZxSpectrumFileSettings zxSpectrumSettings(
  &sdCard0,
  "zxspectrum", ".config"
);
static ZxSpectrumMenu picoRootWin(
  &sdCard0,
  &zxSpectrum,
  &zxSpectrumSettings
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

extern "C" void __not_in_flash_func(process_mouse_report)(hid_mouse_report_t const * report)
{
  // static hid_mouse_report_t prev_report = { 0 };

  // //------------- button state  -------------//
  // uint8_t button_changed_mask = report->buttons ^ prev_report.buttons;
  // if ( button_changed_mask & report->buttons)
  // {
  //   printf(" %c%c%c ",
  //      report->buttons & MOUSE_BUTTON_LEFT   ? 'L' : '-',
  //      report->buttons & MOUSE_BUTTON_MIDDLE ? 'M' : '-',
  //      report->buttons & MOUSE_BUTTON_RIGHT  ? 'R' : '-');
  // }

  // //------------- cursor movement -------------//
  // printf("(%d %d %d)\r\n", report->x, report->y, report->wheel);

  mouse.xDelta(report->x);
  mouse.yDelta(report->y);
  mouse.wDelta(report->wheel);
  mouse.setButtons(report->buttons);
}

extern "C"  void __not_in_flash_func(process_mouse_mount)(uint8_t dev_addr, uint8_t instance) {
//  printf("Mouse connected %d %d\n", dev_addr, instance);
  mouse.mount();
}

extern "C"  void __not_in_flash_func(process_mouse_unmount)(uint8_t dev_addr, uint8_t instance) {
//  printf("Mouse disonnected %d %d\n", dev_addr, instance);
  mouse.unmount();
}

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

void __not_in_flash_func(process_joystick)() {
  int r;
  if (showMenu) {
      r = picoWinHidKeyboard.processJoystick(hidJoystick.joy1());
  }
  else {
      r = picoWinHidKeyboard.processJoystickMenuEnter(hidJoystick.joy1());
  }
  if (r)  {
    toggleMenu = true;
    picoRootWin.repaint();
  }
}

static  PIO pio = pio0;
static  uint sm = 0;

void __not_in_flash_func(setMenuState)(bool showMenu) {
  picomputerJoystick.enabled(!showMenu);  
  pzx_menu_mode(showMenu);
}

#ifdef PICOMPUTER_PICOZX_LCD
void __not_in_flash_func(ZxRgb332RenderLoopCallbackLine)(uint32_t y) {
    pzx_keyscan_row();
}
void __not_in_flash_func(ZxRgb332RenderLoopCallbackMenu)(bool state) {
}
#endif

void __not_in_flash_func(core1_main)() {
  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

#ifdef PICOMPUTER_PICOZX_LCD
  if (useVga) {
    ZxRgb332RenderLoop(
      zxSpectrum,
      _frames,
      showMenu,
      toggleMenu
    );
  }
  // Turn on the LCD backlight
  gpio_init(4);
  gpio_set_dir(4, GPIO_OUT);
  gpio_put(4, 1);
#endif

  picoRootWin.move(0,0,40,30);
  picoRootWin.setWizLayout(0, 12, 18);

  // Start up the LCD
  st7789_init(pio, sm);
  
  sleep_ms(10);

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
        
        _frames++;
      }

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
          zxSpectrum.borderColour(y)); 
      }

      pzx_keyscan_row();
    }
 
    if (toggleMenu) {
      showMenu = !showMenu;
      toggleMenu = false;
      setMenuState(showMenu);
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
      process_joystick();
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
          zxSpectrum.vsync();
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
#ifdef PICOMPUTER_PICOZX_LCD  
  ZxRgb332RenderLoopInit();
#else
  set_sys_clock_khz(200000, true);
#endif

  //Initialise I/O
  stdio_init_all(); 
  
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  
  picoRootWin.refresh([&]() { picoDisplay.refresh(); });
  picoRootWin.snapLoaded([&](const char *name) {
      showMenu = false;
      toggleMenu = false;
      setMenuState(showMenu);
    }
  );
  // TZX tape option handlers
  zxSpectrum.tzxOptionHandlers(
    [&]() { // Clear options
      picoRootWin.clearTzxOptions();
    },
    [&](const char *s) { // Add option
      picoRootWin.addTzxOption(s);
    },
    [&]() { // Show options
      picoRootWin.showTzxOptions();
      showMenu = true;
      toggleMenu = false;
      setMenuState(showMenu);
    }
  );
  picoRootWin.tzxOption(
    [&](uint32_t option) {
      zxSpectrum.tzxOption(option);
      showMenu = false;
      toggleMenu = false;
      setMenuState(showMenu);
    }
  );
  snapFileLoop.set(&picoRootWin);
  quickSave.set(&picoRootWin);

  tusb_init();

  // Configure the GPIO pins for audio
  zxSpectrumAudioInit();

  keyboard1.setZxSpectrum(&zxSpectrum);
  keyboard2.setZxSpectrum(&zxSpectrum);

  // Initialise the menu renderer
  pcw_init_renderer();

  // Initialise the keyboard scan
  pzx_keyscan_init();

#ifdef PICOMPUTER_PICOZX_LCD
  pcw_init_vga332_renderer();
  useVga = pzx_fire_raw();
  ZxSpectrumFatSpiExists vgaoption(&sdCard0, "zxspectrum", "vga.txt");
  useVga |= vgaoption.exists();  
#endif

  sem_init(&dvi_start_sem, 0, 1);
  
  multicore_launch_core1(core1_main);
  
  picoRootWin.showMessage([=](PicoPen *pen) {
    pen->printAtF(3, 1, false, "Reading from SD card...");
  });
          
  picoDisplay.refresh();
  
  sem_release(&dvi_start_sem);

  if (sdCard0.mount()) {

    // Create folders on the SD card if they are missing
    picoRootWin.initialise();
        
    // Load quick save slot 1 if present
    quickSave.load(&zxSpectrum, 0);
  
    // See if the board is in kiosk mode    
    bool isKiosk = zxSpectrumKisok.isKiosk();
    keyboard1.setKiosk(isKiosk);
    keyboard2.setKiosk(isKiosk);
  }
  
  showMenu = false;
  picoRootWin.removeMessage();
  
  main_loop();
}
