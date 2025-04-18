#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "PicoCoreVoltage.h"
#include "hardware/pwm.h"

#include "pzx_keyscan.h"

#include "PicoCharRendererVga.h"
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
#include "ZxScanlineVgaRenderLoop.h"
#include "hid_app.h"

#define LED_PIN 25

#define VREG_VSEL VREG_VOLTAGE_1_20

struct semaphore dvi_start_sem;

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
    picoWinHidKeyboard.cancelRepeat();
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
  if (r) {
    toggleMenu = true;
    picoRootWin.repaint();
    picoWinHidKeyboard.cancelRepeat();
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
    picoWinHidKeyboard.cancelRepeat();
  }
}

void __not_in_flash_func(ZxRenderLoopCallbackLine)(int32_t y) {
    pzx_keyscan_row();
}

void __not_in_flash_func(ZxRenderLoopCallbackMenu)(bool state) {
  picomputerJoystick.enabled(!showMenu);  
  pzx_menu_mode(showMenu);
}

void core1_main() {

  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

  ZxScanlineVgaRenderLoop(
    zxSpectrum,
    _frames,
    showMenu,
    toggleMenu
  );

  __builtin_unreachable();
}

void __not_in_flash_func(main_loop)(){

  unsigned int lastInterruptFrame = _frames;

  //Main Loop 
  uint frames = 0;
  
  while(1){
    
    tuh_task();

    hid_keyboard_report_t const *curr;
    hid_keyboard_report_t const *prev;
    pzx_keyscan_get_hid_reports(&curr, &prev);
    process_picomputer_kbd_report(curr, prev);
    process_joystick();
    
    if (!showMenu) {
      for (int i = 1; i < 100; ++i) {
        if (lastInterruptFrame != _frames) {
          lastInterruptFrame = _frames;
          zxSpectrum.vsync();
        }
        zxSpectrum.step();
      }
    }
    else {
      picoWinHidKeyboard.processKeyRepeat();
      if (frames != _frames) {
        frames = _frames;
        picoDisplay.refresh();
      }
    }
  }
}

int main(){
  pico_set_core_voltage();

  ZxScanlineVgaRenderLoopInit();

  //Initialise I/O
  stdio_init_all(); 
  
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  
  picoRootWin.refresh([&]() { picoDisplay.refresh(); });
  picoRootWin.snapLoaded([&](const char *name) {
      toggleMenu = showMenu;
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
      toggleMenu = !showMenu;
    }
  );
  picoRootWin.tzxOption(
    [&](uint32_t option) {
      zxSpectrum.tzxOption(option);
      toggleMenu = showMenu;
    }
  );
  snapFileLoop.set(&picoRootWin);
  quickSave.set(&picoRootWin);
  
  tuh_hid_app_startup();

  zxSpectrum.setAudioDriver(zxSpectrumAudioInit(PICO_DEFAULT_AUDIO));

  keyboard1.setZxSpectrum(&zxSpectrum);
  keyboard2.setZxSpectrum(&zxSpectrum);
  
  // Initialise the menu renderer
  pcw_init_renderer();
  
  // Initialise the keyboard scan
  pzx_keyscan_init();
  
  sleep_ms(10);
  
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
