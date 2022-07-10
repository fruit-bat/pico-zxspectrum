#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/ssi.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "pico/sem.h"
#include "hardware/pwm.h"
#include "ps2kbd.h"

extern "C" {
#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"
#include "tmds_encode_zxspectrum.h"
}
#include "ZxSpectrumFatSpiKiosk.h"
#include "ZxSpectrum.h"
#include "ZxSpectrumHidKeyboard.h"
#include "ZxSpectrumHidJoystick.h"

#include "bsp/board.h"
#include "tusb.h"
#include <pico/printf.h>
#include "SdCardFatFsSpi.h"
#include "QuickSave.h"
#include "ZxSpectrumFatFsSpiFileLoop.h"

#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "PicoCharRenderer.h"
#include "ZxSpectrumMenu.h"
#include "ZxSpectrumAudio.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// DVDD 1.2V (1.1V seems ok too)
#define FRAME_HEIGHT 240
#define VREG_VSEL VREG_VOLTAGE_1_20
#define DVI_TIMING dvi_timing_640x480p_60hz

#define LED_PIN 25

struct dvi_inst dvi0;
struct semaphore dvi_start_sem;

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
static ZxSpectrumHidKeyboard keyboard(
  &zxSpectrumSnaps,
  &zxSpectrumTapes,
  &quickSave,
  &joystick
);
static ZxSpectrum zxSpectrum(
  &keyboard, 
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
  keyboard.mount();
}

extern "C"  void __not_in_flash_func(process_kbd_unmount)(uint8_t dev_addr, uint8_t instance) {
  keyboard.unmount();
}

extern "C"  void __not_in_flash_func(process_kbd_report)(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report) {
	printf("CURR ");print(report);
	printf("PREV ");print(prev_report);
	
  int r;
  if (showMenu) {
    r = picoWinHidKeyboard.processHidReport(report, prev_report);
  }
  else {
    r = keyboard.processHidReport(report, prev_report);
  }
  if (r == 1) toggleMenu = true;
}

static Ps2Kbd ps2kbd(
  pio1,
  6,
  process_kbd_report
);

unsigned char* screenPtr;
unsigned char* attrPtr;
static volatile uint _frames = 0;

// Screen handler
//
// 256×192 Pixels
// 32x24 Charaters
//
// 240-192 = 48 => 24 border rows top and bottom
// 320-256 = 64 => 64 border pixels left and right
//
static inline void prepare_scanline(const uint y, uint f) {
  
  const uint8_t borderColor = zxSpectrum.borderColour();
  uint32_t *tmdsbuf;
  queue_remove_blocking(&dvi0.q_tmds_free, &tmdsbuf);
  uint32_t *p = tmdsbuf;
  const uint v = y - 24;
  const uint8_t *s = screenPtr + ((v & 0x7) << 8) + ((v & 0x38) << 2) + ((v & 0xc0) << 5);
  const uint8_t *a = attrPtr+((v>>3)<<5);
  const int m = (f >> 5) & 1 ? 0xff : 0x7f;
  
  if (y < 24 || y >= (24+192)) {
    for (int plane = 0; plane < 3; ++plane) {
      p = tmds_encode_border(
        40,          // r0 is width in characters
        plane,       // r1 is colour channel
        p,           // r2 is output TMDS buffer
        borderColor  // r3 is the colour attribute
      );    
    }
  }
  else {
    for (int plane = 0; plane < 3; ++plane) {
      p = tmds_encode_border(
        4,           // r0 is width in characters
        plane,       // r1 is colour channel
        p,           // r2 is output TMDS buffer
        borderColor  // r3 is the colour attribute
      );
      p = tmds_encode_screen(
        (const uint8_t*)s,
        a,
        p,
        m,
        plane
      );  
      p = tmds_encode_border(
        4,           // r0 is width in characters
        plane,       // r1 is colour channel
        p,           // r2 is output TMDS buffer
        borderColor  // r3 is the colour attribute
      );
    }
  }
  queue_add_blocking(&dvi0.q_tmds_valid, &tmdsbuf);
}

void __not_in_flash_func(core1_scanline_callback)() {
  static uint y = 1;
  static uint ys = 0;
  if (showMenu) {
    uint rs = pcw_prepare_scanline_80(&dvi0, y++, ys, _frames);
    if (0 == (y & 7)) {
      ys += rs;
    }
  }
  else {
    prepare_scanline(y++, _frames);
  }
  if (y == FRAME_HEIGHT) {
    _frames++;
    y = 0;
    ys = 0;
    
    // TODO Tidy this mechanism up
    screenPtr = zxSpectrum.screenPtr();
    attrPtr = screenPtr + (32 * 24 * 8);
    
    if (toggleMenu) {
      showMenu = !showMenu;
      toggleMenu = false;
    }
  }
}

void __not_in_flash_func(core1_main)() {
  dvi_register_irqs_this_core(&dvi0, DMA_IRQ_1);
  sem_acquire_blocking(&dvi_start_sem);

  dvi_start(&dvi0);

  // The text display is completely IRQ driven (takes up around 30% of cycles @
  // VGA). We could do something useful, or we could just take a nice nap
  while (1) 
    __wfi();
  __builtin_unreachable();
}

void __not_in_flash_func(main_loop)() {
  
  unsigned int lastInterruptFrame = _frames;
  
  uint frames = 0;
  
  while (1) {
    tuh_task();
    ps2kbd.tick();
    for (int i = 1; i < 100; ++i) {
      if (lastInterruptFrame != _frames) {
        lastInterruptFrame = _frames;
        zxSpectrum.interrupt();
      }
      zxSpectrum.step();
      zxSpectrumAudioToGpio(zxSpectrum);
    }
    if (showMenu && frames != _frames) {
      frames = _frames;
      picoDisplay.refresh();
    }
  }
}

int main() {
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);
#ifdef RUN_FROM_CRYSTAL
  set_sys_clock_khz(12000, true);
#else
  // Run system at TMDS bit clock
  set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);
#endif

  setup_default_uart();
  tusb_init();
  ps2kbd.init_gpio();

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  
  // Configure the GPIO pins for audio
  zxSpectrumAudioInit();
 
  screenPtr = zxSpectrum.screenPtr();
  attrPtr = screenPtr + (32 * 24 * 8);

  keyboard.setZxSpectrum(&zxSpectrum);

  // Initialise the menu renderer
  pcw_init_renderer();
  
  printf("Configuring DVI\n");
  dvi0.timing = &DVI_TIMING;
  dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
  dvi0.scanline_callback = core1_scanline_callback;
  dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

  printf("Prepare first scanline\n");

  prepare_scanline(0, 0);

  printf("Core 1 start\n");
  sem_init(&dvi_start_sem, 0, 1);
  hw_set_bits(&bus_ctrl_hw->priority, BUSCTRL_BUS_PRIORITY_PROC1_BITS);
  
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
    
    bool isKiosk = zxSpectrumKisok.isKiosk();
    keyboard.setKiosk(isKiosk);
  }

  showMenu = false;
  picoRootWin.removeMessage();

  main_loop();
  
  __builtin_unreachable();
}
