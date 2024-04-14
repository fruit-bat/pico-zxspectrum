#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"
#ifdef USE_PS2_KBD
#include "ps2kbd.h"
#endif
#include "ZxSpectrumPrepareScanvideoScanline.h"
#include "PicoCharRendererScanvideo.h"
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
#define SPK_PIN 9

// https://tomverbeure.github.io/video_timings_calculator
// CEA-861
const scanvideo_timing_t vga_timing_768x576_50 =
{
  .clock_freq = 27000000,

  .h_active = 720,
  .v_active = 576,

  .h_front_porch = 12,
  .h_pulse = 64,
  .h_total = 864,
  .h_sync_polarity = 1,

  .v_front_porch = 5,
  .v_pulse = 5,
  .v_total = 625,
  .v_sync_polarity = 1,

  .enable_clock = 0,
  .clock_polarity = 0,

  .enable_den = 0
};

const scanvideo_mode_t vga_mode_640x240_60 =
{
  .default_timing = &vga_timing_640x480_60_default,
  .pio_program = &video_24mhz_composable,
  .width = 640,
  .height = 240,
  .xscale = 1,
  .yscale = 2,
};

const scanvideo_mode_t vga_mode_768x288_50 =
{
  .default_timing = &vga_timing_768x576_50,
  .pio_program = &video_24mhz_composable,
  .width = 720,
  .height = 576,
  .xscale = 1,
  .yscale = 2,
};

#ifndef VGA_MODE
#define VGA_MODE vga_mode_640x240_60
#endif  
#define VREG_VSEL VREG_VOLTAGE_1_20

struct semaphore dvi_start_sem;

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

#ifdef USE_PS2_KBD
static Ps2Kbd ps2kbd(
  pio1,
  6,
  process_kbd_report
);
#endif

unsigned char* screenPtr;
unsigned char* attrPtr;
static volatile uint _frames = 0;

void __not_in_flash_func(core1_main)() {
  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

  
  scanvideo_setup(&VGA_MODE);
  scanvideo_timing_enable(true);

  int core_num = get_core_num();
  printf("Rendering on core %d\n", core_num);
  while (true) {
    struct scanvideo_scanline_buffer *scanline_buffer = scanvideo_begin_scanline_generation(true);
    uint32_t frame_num = scanvideo_frame_number(scanline_buffer->scanline_id);
    uint32_t y = scanvideo_scanline_number(scanline_buffer->scanline_id);

    const uint32_t fpf = zxSpectrum.flipsPerFrame();
    const bool ringoMode = fpf > 46 && fpf < 52;
    if (ringoMode) {
      screenPtr = zxSpectrum.memPtr(y & 4 ? 7 : 5);
      attrPtr = screenPtr + (32 * 24 * 8);
    }

    if (false && showMenu) {
      pcw_prepare_scanvideo_scanline_80(
        scanline_buffer,
        y,
        frame_num);
    }
    else { 
      zx_prepare_scanvideo_scanline(
        scanline_buffer, 
        y, 
        frame_num,
        screenPtr,
        attrPtr,
        zxSpectrum.borderColour(y)
      );
    }

    // Release the rendered buffer into the wild
    scanvideo_end_scanline_generation(scanline_buffer);
    
    if (y == 239) {
      
      if (!ringoMode) {
        screenPtr = zxSpectrum.screenPtr();
        attrPtr = screenPtr + (32 * 24 * 8);
      }
      
      if (toggleMenu) {
        showMenu = !showMenu;
        toggleMenu = false;
//        picomputerJoystick.enabled(!showMenu);
      }
      
      _frames = frame_num;
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
}

int main(){
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);
  // TODO init 16 bit VGA
  set_sys_clock_khz(VGA_MODE.default_timing->clock_freq / 100, true);
  sleep_ms(100);

  //Initialise I/O
  stdio_init_all(); 
  
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

  screenPtr = zxSpectrum.screenPtr();
  attrPtr = screenPtr + (32 * 24 * 8);

  keyboard1.setZxSpectrum(&zxSpectrum);
//  keyboard2.setZxSpectrum(&zxSpectrum);
  
  // Initialise the menu renderer
  pcw_init_renderer();

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
