#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"
//---------------------------
#include "hardware/timer.h"
//#include "ps2kbd.h"
#include "ps2kbd_mrmltr.h"
//---------------------------
#include "vga.h"
#include "ZxSpectrumPrepareRgbScanline.h"

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

//=============================================================================
#define LED_PIN 25 //25
#define VREG_VSEL VREG_VOLTAGE_1_10

struct semaphore dvi_start_sem;
static const sVmode* vmode = NULL;

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
static ZxSpectrumMenu picoRootWin(
  &sdCard0,
  &zxSpectrum
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

//=============================================================================
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
static Ps2Kbd_Mrmltr ps2kbd(
  pio1,
  0,
  process_kbd_report
);
#endif

//=============================================================================
unsigned char* screenPtr;
unsigned char* attrPtr;
static volatile uint _frames = 0;
// MADEIT for ZX-Spectrum Z80_INT_Interrupt = 50Hz
static volatile uint8_t LineBorderSync = 0;   // MADEIT for BorderSync
static volatile uint8_t borderBuf[240];       //Border Buffer 240 lines
//-----------------------------------------------------------------------------
void __not_in_flash_func(core1_main)() {
  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

  // TODO fetch the resolution from the mode ?
  VgaInit(vmode,640,480);

  while (1) {
    //-------------------------------------------------------------------------*/
    VgaLineBuf *linebuf = get_vga_line();
    uint32_t* buf = (uint32_t*)&(linebuf->line);
    uint32_t y = linebuf->row;
    _frames = linebuf->frame;

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
        //zxSpectrum.borderColour()
        borderBuf[y]
      );
    }
    //-----------------------------------------------------
    if (y == 239) { // TODO use a const / get from vmode

      // TODO Tidy this mechanism up
      screenPtr = zxSpectrum.screenPtr();
      attrPtr = screenPtr + (32 * 24 * 8);
      
      if (toggleMenu) {
        showMenu = !showMenu;
        toggleMenu = false;
        //picomputerJoystick.enabled(!showMenu);
      }      
    }
  }
  __builtin_unreachable();
}

//=============================================================================
#ifdef EAR_PIN
#define CPU_STEP_LOOP 10
#else
#define CPU_STEP_LOOP 100
#endif
//-----------------------------------------------------------------------------
void __not_in_flash_func(main_loop)(){

//  unsigned int lastInterruptFrame = _frames;
  uint _INTcounter = 0;

  //Main Loop 
  uint frames = 0;
  //-------------------------------------------------------------------  
  while(1){

    tuh_task();
#ifdef USE_PS2_KBD
    ps2kbd.tick();
#endif
    //-----------------------------------------------------------------  
    if (!showMenu) {
      for (int i = 1; i < CPU_STEP_LOOP; ++i) {
        //---------------------------------------
        /*// Z80 INT Interrupt = 60Hz
        if (lastInterruptFrame != _frames) {
          lastInterruptFrame = _frames;
          zxSpectrum.interrupt();   
        }
        //-------------------------------------*/
        // MADEIT for ZX-Spectrum Z80_INT_Interrupt = 50Hz (ZX128=625;Pent128=628)
        if (_INTcounter++ >= 625) {
          _INTcounter = 0;
          LineBorderSync = 0;
          zxSpectrum.interrupt();
        }

        if (_INTcounter >= 78) {
          if (LineBorderSync<240) {
            if (_INTcounter & 1) {borderBuf[LineBorderSync++] = zxSpectrum.borderColour();}
          }
        }
        //-------------------------------------*/
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
//-------------------------------------------------------------------
  }
  __builtin_unreachable();
}

//=============================================================================
int main(){
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);
  vmode = Video(DEV_VGA, RES_HVGA);
  sleep_ms(100);

  //Initialise I/O
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
  pcw_init_vga332_renderer();
  
  sleep_ms(10);
  
  sem_init(&dvi_start_sem, 0, 1);
  
  multicore_launch_core1(core1_main);

  picoRootWin.showMessage([=](PicoPen *pen) {
    pen->printAtF(3, 1, false, "Reading from SD card...");
  });
          
  picoDisplay.refresh();
  
  sem_release(&dvi_start_sem);
 
  if (sdCard0.mount()) {

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
