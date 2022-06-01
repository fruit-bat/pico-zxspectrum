#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/vreg.h"
#include "hardware/pwm.h"


#include "pzx_keyscan.h"

#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "PicoPen.h"
#include "PicoTextField.h"
#include "PicoWinHidKeyboard.h"


#include "ZxSpectrum.h"
#include "ZxSpectrumHidKeyboard.h"
#include "ZxSpectrumDualJoystick.h"
#include "ZxSpectrumHidJoystick.h"
#include "ZxSpectrumPicomputerVgaJoystick.h"

#include "bsp/board.h"
#include "tusb.h"
#include <pico/printf.h>
#include "SdCardFatFsSpi.h"
#include "QuickSave.h"
#include "ZxSpectrumFatFsSpiFileLoop.h"

#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "ZxSpectrumMenu.h"

#define LED_PIN 25
#define SPK_PIN 0
#define PWM_WRAP (256 + 256 + 256 + 256 + 256)
#define PWM_MID (PWM_WRAP>>1)

#define VREG_VSEL VREG_VOLTAGE_1_10

struct semaphore dvi_start_sem;
static const sVmode* vmode = NULL;

uint8_t* screenPtr;
uint8_t* attrPtr;

static SdCardFatFsSpi sdCard0(0);

// ZX Spectrum emulator
static QuickSave quickSave(&sdCard0, "zxspectrum/quicksaves");
static ZxSpectrumHidJoystick hidJoystick;
static ZxSpectrumPicomputerVgaJoystick picomputerVgaJoystick;
static ZxSpectrumDualJoystick dualJoystick(&hidJoystick, &picomputerVgaJoystick);

static ZxSpectrumHidKeyboard keyboard1(
  &quickSave, 
  &dualJoystick
);
static ZxSpectrumHidKeyboard keyboard2(
  &quickSave, 
  &picomputerVgaJoystick
);
static ZxSpectrum zxSpectrum(
  &keyboard1, 
  &keyboard2, 
  &dualJoystick
);

// Menu system
static ZxSpectrumMenu picoRootWin(
  &sdCard0, 
  &zxSpectrum, 
  &quickSave
);

static PicoDisplay picoDisplay(pcw_screen(), &picoRootWin);
static PicoWinHidKeyboard picoWinHidKeyboard(&picoDisplay);

static bool showMenu = true;
static bool toggleMenu = false;
static volatile uint _frames = 0;

extern "C"  void __not_in_flash_func(process_kbd_report)(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report) {
  int r;
  if (showMenu) {
    r = picoWinHidKeyboard.processHidReport(report, prev_report);
  }
  else {
    r = keyboard1.processHidReport(report, prev_report);
  }
  if (r == 1) toggleMenu = true;
}

void __not_in_flash_func(process_picomputer_kbd_report)(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report) {
  int r;
  if (showMenu) {
    r = picoWinHidKeyboard.processHidReport(report, prev_report);
  }
  else {
    r = keyboard2.processHidReport(report, prev_report);
  }
  if (r == 1) toggleMenu = true;
}

void __not_in_flash_func(core1_main)() {
  sem_acquire_blocking(&dvi_start_sem);
  printf("Core 1 running...\n");

  // TODO fetch the resolution from the mode ?
  // VgaInit(vmode,640,480);

  while (1) {
/*
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
      pzx_prepare_vga332_scanline(
        buf, 
        y, 
        linebuf->frame,
        screenPtr,
        attrPtr,
        zxSpectrum.borderColour()
      );
    }
  */    
    pzx_keyscan_row();
    
    if (y == 239) { // TODO use a const / get from vmode
      
      // TODO Tidy this mechanism up
      screenPtr = zxSpectrum.screenPtr();
      attrPtr = screenPtr + (32 * 24 * 8);
      
      if (toggleMenu) {
        showMenu = !showMenu;
        toggleMenu = false;
      }
      
      _frames = linebuf->frame;
    }    
  }
  __builtin_unreachable();
}

int main(){
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);
  // vmode = Video(DEV_VGA, RES_HVGA);
  sleep_ms(100);

  //Initialise I/O
  stdio_init_all(); 
  
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // Don't use the same DMA channel as the screen  
  set_spi_dma_irq_channel(true, true);

  tusb_init();

  gpio_set_function(SPK_PIN, GPIO_FUNC_PWM);
  const int audio_pin_slice = pwm_gpio_to_slice_num(SPK_PIN);
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 1.0f); 
  pwm_config_set_wrap(&config, PWM_WRAP);
  pwm_init(audio_pin_slice, &config, true);

  screenPtr = zxSpectrum.screenPtr();
  attrPtr = screenPtr + (32 * 24 * 8);

  keyboard1.setZxSpectrum(&zxSpectrum);
  keyboard2.setZxSpectrum(&zxSpectrum);

  // Initialise the menu renderer
  pcw_init_renderer();

  // Initialise the keyboard scan
  pzx_keyscan_init();
  
  sleep_ms(10);
  
  sem_init(&dvi_start_sem, 0, 1);
  
  multicore_launch_core1(core1_main);
    
  sem_release(&dvi_start_sem);

  unsigned int lastInterruptFrame = _frames;

  //Main Loop 
  uint frames = 0;  
  while(1){
    
    tuh_task();

    hid_keyboard_report_t const *curr;
    hid_keyboard_report_t const *prev;
    pzx_keyscan_get_hid_reports(&curr, &prev);
    process_picomputer_kbd_report(curr, prev);
    
    for (int i = 1; i < 100; ++i) {
      if (lastInterruptFrame != _frames) {
        lastInterruptFrame = _frames;
        zxSpectrum.interrupt();
      }
      zxSpectrum.step();
      const uint32_t l = zxSpectrum.getSpeaker();
      pwm_set_gpio_level(SPK_PIN, PWM_MID + l);

    }

    if (showMenu && frames != _frames) {
      frames = _frames;
      picoDisplay.refresh();
    }
      
  }
}
