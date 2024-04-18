#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"
#ifdef USE_MRMLTR_PS2_KBD
#include "ps2kbd_mrmltr.h"
#else
#include "ps2kbd.h"
#endif
#include "vga.h"
#include "ZxSpectrumPrepareRgbScanline.h"
// #include "pzx_keyscan.h"

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
// #include "ZxSpectrumPicomputerJoystick.h"

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
#include "ZxSpectrumDisplay.h"

#define LED_PIN 25

#define VREG_VSEL VREG_VOLTAGE_1_20

struct semaphore dvi_start_sem;
static sVmode vmode;

static SdCardFatFsSpi sdCard0(0);

// ZX Spectrum emulator
static ZxSpectrumFatSpiKiosk zxSpectrumKisok(
  &sdCard0,
  "zxspectrum"
);
static ZxSpectrumFileLoop snapFileLoop;
static QuickSave quickSave;
static ZxSpectrumHidJoystick joystick;
static ZxSpectrumHidKeyboard keyboard1(
  &snapFileLoop,
  &quickSave,
  &joystick
);
static ZxSpectrum zxSpectrum(
  &keyboard1, 
  0, 
  &joystick
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

void print(hid_keyboard_report_t const *report) {
  printf("HID key report modifiers %2.2X report ", report->modifier);
  for(int i = 0; i < 6; ++i) printf("%2.2X", report->keycode[i]);
  printf("\n");
}

extern "C"  void __not_in_flash_func(process_kbd_mount)(uint8_t dev_addr, uint8_t instance) {
  keyboard1.mount();
}

extern "C"  void __not_in_flash_func(process_kbd_unmount)(uint8_t dev_addr, uint8_t instance) {
  keyboard1.unmount();
}

extern "C"  void __not_in_flash_func(process_kbd_report)(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report) {
#if 0
  // Some help debugging keyboard input/drivers
  printf("PREV ");print(prev_report);
  printf("CURR ");print(report);
#endif

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

#ifdef USE_MRMLTR_PS2_KBD
static Ps2Kbd_Mrmltr ps2kbd(
  pio1,
  8,
  process_kbd_report
);
#elif defined(USE_PS2_KBD)
static Ps2Kbd ps2kbd(
  pio1,
  6,
  process_kbd_report
);
#endif


static volatile uint _frames = 0;

void __not_in_flash_func(setMenuState)(bool showMenu) {
//  picomputerJoystick.enabled(!showMenu);  
//  pzx_menu_mode(showMenu);
}

void __not_in_flash_func(core1_main)() {
  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

  VgaInit(&vmode);

  unsigned char* screenPtr = zxSpectrum.screenPtr();
  unsigned char* attrPtr = screenPtr + (32 * 24 * 8);

  while (true) {
    VgaLineBuf *linebuf = get_vga_line();
    uint32_t* buf = (uint32_t*)&(linebuf->line);
    uint32_t y = linebuf->row;

    const uint32_t fpf = zxSpectrum.flipsPerFrame();
    const bool ringoMode = fpf > 46 && fpf < 52;
    if (ringoMode) {
      screenPtr = zxSpectrum.memPtr(y & 4 ? 7 : 5);
      attrPtr = screenPtr + (32 * 24 * 8);
    }
    const uint32_t blankTopLines = (DISPLAY_BLANK_LINES/2);

    if (y == blankTopLines) {
      
      if (!ringoMode) {
        screenPtr = zxSpectrum.screenPtr();
        attrPtr = screenPtr + (32 * 24 * 8);
      }
      
      if (toggleMenu) {
        showMenu = !showMenu;
        toggleMenu = false;
        setMenuState(showMenu);
      }
      
      _frames = linebuf->frame;
    }

    if (y < blankTopLines || y >= (blankTopLines + ZX_SPECTRUM_SCREEN_HEIGHT)) {
      zx_prepare_rgb_blankline(
        buf
      );
    }
    else if (showMenu) {
      pcw_prepare_vga332_scanline_80(
        buf,
        y - blankTopLines,
        linebuf->frame);
    }
    else { 
      zx_prepare_rgb_scanline(
        buf, 
        y - blankTopLines, 
        linebuf->frame,
        screenPtr,
        attrPtr,
        zxSpectrum.borderColour(y - blankTopLines)
      );

    }  
  }

  __builtin_unreachable();
}

#define CPU_STEP_LOOP 100

void __not_in_flash_func(main_loop)(){

  unsigned int lastInterruptFrame = _frames;

  //Main Loop 
  uint frames = 0;
  
  while(1){
    
    tuh_task();
#ifdef USE_PS2_KBD
    ps2kbd.tick();
#endif

//    hid_keyboard_report_t const *curr;
//    hid_keyboard_report_t const *prev;
//    pzx_keyscan_get_hid_reports(&curr, &prev);
//    process_picomputer_kbd_report(curr, prev);
    
    if (!showMenu) {
      for (int i = 1; i < CPU_STEP_LOOP; ++i) {
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
  __builtin_unreachable();
}

int main(){
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);

  sVgaCfg cfg;
  cfg.width = DISPLAY_WIDTH_PIXELS;       // width in pixels
  cfg.height = DISPLAY_HEIGHT_PIXELS / 2; // height in lines
  cfg.wfull = 0;                          // width of full screen, corresponding to 'hfull' time (0=use 'width' parameter)
  cfg.video = &VGA_MODE;                  // used video timings
  cfg.freq = 180000;                      // required minimal system frequency in kHz (real frequency can be higher)
  cfg.fmax = 280000;                      // maximal system frequency in kHz (limit resolution if needed)
  cfg.dbly = true;                        // double in Y direction
  cfg.lockfreq = false;                   // lock required frequency, do not change it
  VgaCfg(&cfg, &vmode);                   // calculate videomode setup

  // initialize system clock
  set_sys_clock_pll(vmode.vco * 1000, vmode.pd1, vmode.pd2);
//set_sys_clock_khz(270000 / 100, true);

  sleep_ms(100);

#ifdef USE_STDIO
  //Initialise I/O
  stdio_init_all(); 
#endif

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  
  picoRootWin.refresh([&]() { picoDisplay.refresh(); });
  picoRootWin.snapLoaded([&](const char *name) {
      showMenu = false;
      toggleMenu = false;
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
    }
  );
  picoRootWin.tzxOption(
    [&](uint32_t option) {
      zxSpectrum.tzxOption(option);
      showMenu = false;
      toggleMenu = false;
    }
  );
  snapFileLoop.set(&picoRootWin);
  quickSave.set(&picoRootWin);
  
  tusb_init();
#ifdef USE_PS2_KBD
  ps2kbd.init_gpio();
#endif

  // Configure the GPIO pins for audio
  zxSpectrumAudioInit();

  keyboard1.setZxSpectrum(&zxSpectrum);
//  keyboard2.setZxSpectrum(&zxSpectrum);
  
  // Initialise the menu renderer
  pcw_init_renderer();
  pcw_init_vga332_renderer();
  
  // Initialise the keyboard scan
//  pzx_keyscan_init();
  
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
//    keyboard2.setKiosk(isKiosk);
  }

  showMenu = false;
  picoRootWin.removeMessage();
  
  main_loop();
}
