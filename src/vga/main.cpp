#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"
#include "ps2kbd.h"

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
#include "ZxSpectrumFatFsSpiFileLoop.h"

#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "ZxSpectrumMenu.h"
#include "ZxSpectrumAudio.h"

#define LED_PIN 25
#define SPK_PIN 9

#define VREG_VSEL VREG_VOLTAGE_1_10

struct semaphore dvi_start_sem;
static const sVmode* vmode = NULL;

static SdCardFatFsSpi sdCard0(0);

// ZX Spectrum emulator
static ZxSpectrumFatSpiKiosk zxSpectrumKisok(
  &sdCard0,
  "zxspectrum"
);
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
static ZxSpectrumHidJoystick joystick;
static ZxSpectrumHidKeyboard keyboard1(
  &zxSpectrumSnaps,
  &zxSpectrumTapes,
  &quickSave,
  &joystick
);
static ZxSpectrum zxSpectrum(
  &keyboard1, 
  0, 
  &joystick
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

  // TODO fetch the resolution from the mode ?
  VgaInit(vmode,640,480);

  while (1) {

    VgaLineBuf *linebuf = get_vga_line();
    uint32_t* buf = (uint32_t*)&(linebuf->line);
    uint32_t y = linebuf->row;
    if (showMenu) {
      pcw_prepare_vga332_scanline_80(
        buf,
        y,
        linebuf->frame);
    }
    else {
      zx_prepare_rgb_scanline(
        buf, 
        y, 
        linebuf->frame,
        screenPtr,
        attrPtr,
        zxSpectrum.borderColour()
      );
    }
      
//    pzx_keyscan_row();
    
    if (y == 239) { // TODO use a const / get from vmode
      
      // TODO Tidy this mechanism up
      screenPtr = zxSpectrum.screenPtr();
      attrPtr = screenPtr + (32 * 24 * 8);
      
      if (toggleMenu) {
        showMenu = !showMenu;
        toggleMenu = false;
//        picomputerJoystick.enabled(!showMenu);
      }
      
      _frames = linebuf->frame;
    }    
  }
  __builtin_unreachable();
}

#ifdef EAR_PIN
#define CPU_STEP_LOOP 10
#else
#define CPU_STEP_LOOP 100
#endif

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
          zxSpectrum.interrupt();
        }
#ifdef EAR_PIN
        if (zxSpectrum.moderate()) {
          zxSpectrum.step(zxSpectrumReadEar());
        }
        else {
          zxSpectrum.step();
        }
#else
        zxSpectrum.step();
#endif 
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
  vmode = Video(DEV_VGA, RES_HVGA);
  sleep_ms(100);

  //Initialise I/O
  stdio_init_all(); 
  
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

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
//    keyboard2.setKiosk(isKiosk);
	}

  showMenu = false;
  picoRootWin.removeMessage();
  
  main_loop();
}
