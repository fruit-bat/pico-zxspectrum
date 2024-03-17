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

#ifdef USE_PS2_KBD
#include "ps2kbd.h"
#endif

#ifdef USE_KEY_MATRIX
#include "ZxSpectrumKeyMatrix.h"
#endif

extern "C" {
#include "dvi.h"
#include "dvi_serialiser.h"
#ifdef DVI_CUSTOM_CONFIG
#include "custom_dvi_pin_configs.h"
#else
#include "common_dvi_pin_configs.h"
#endif
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
#include "ZxSpectrumFileLoop.h"
#include "ZxSpectrumPrepareDviScanline.h"
#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "PicoCharRenderer.h"
#include "ZxSpectrumMenu.h"
#include "ZxSpectrumAudio.h"
#include "ZxSpectrumFileSettings.h"


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
static ZxSpectrumFileLoop snapFileLoop;
static QuickSave quickSave;
static ZxSpectrumHidJoystick joystick;
static ZxSpectrumHidKeyboard keyboard1(
  &snapFileLoop,
  &quickSave,
  &joystick
);
static ZxSpectrumHidKeyboard keyboard2(
  &snapFileLoop, 
  &quickSave, 
  0
);
static ZxSpectrum zxSpectrum(
  &keyboard1, 
  &keyboard2,  
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

void __not_in_flash_func(core1_render)() {
  static uint y = 0;
  static uint ys = 0;
  while(true) {
    if (showMenu) {
      uint rs = pcw_prepare_scanline_80(&dvi0, y++, ys, _frames);
      if (0 == (y & 7)) {
        ys += rs;
      }
    }
    else {
      zx_prepare_hdmi_scanline(&dvi0, y++, _frames, screenPtr, attrPtr, zxSpectrum.borderColour());
    }
  #ifdef USE_KEY_MATRIX
  // zx_keyscan_row();
  #endif
    if (y == FRAME_HEIGHT) {
      y = 0;
      ys = 0;
      _frames++;
      // TODO Tidy this mechanism up
      screenPtr = zxSpectrum.screenPtr();
      attrPtr = screenPtr + (32 * 24 * 8);
      
      if (toggleMenu) {
        showMenu = !showMenu;
        toggleMenu = false;
      }
    }
  }
}

//Audio Related
#define AUDIO_BUFFER_SIZE   256
audio_sample_t      audio_buffer[AUDIO_BUFFER_SIZE];
struct repeating_timer audio_timer;
const int16_t sine[32] = {
    (short)0x8000,(short)0x98f8,(short)0xb0fb,(short)0xc71c,(short)0xda82,(short)0xea6d,(short)0xf641,(short)0xfd89,
    (short)0xffff,(short)0xfd89,(short)0xf641,(short)0xea6d,(short)0xda82,(short)0xc71c,(short)0xb0fb,(short)0x98f8,
    (short)0x8000,(short)0x6707,(short)0x4f04,(short)0x38e3,(short)0x257d,(short)0x1592,(short)0x9be,(short)0x276,
    (short)0x0000,(short)0x276,(short)0x9be,(short)0x1592,(short)0x257d,(short)0x38e3,(short)0x4f04,(short)0x6707
};

bool __not_in_flash_func(audio_timer_callback)(struct repeating_timer *t) {
    int size = get_write_size(&dvi0.audio_ring, false);
    audio_sample_t *audio_ptr = get_write_pointer(&dvi0.audio_ring);
    audio_sample_t sample;
    static uint sample_count = 0;
    for (int cnt = 0; cnt < size; cnt++) {
        sample.channels[0] = sine[sample_count % 32] /8;
        sample.channels[1] = sine[sample_count % 32] /8;
        *audio_ptr++ = sample;
        sample_count++;
    }
    increase_write_pointer(&dvi0.audio_ring, size);
 
    return true;
}

void __not_in_flash_func(core1_main)() {
  
  dvi_register_irqs_this_core(&dvi0, DMA_IRQ_1);
  sem_acquire_blocking(&dvi_start_sem);
  dvi_start(&dvi0);

core1_render();
  // The text display is completely IRQ driven (takes up around 30% of cycles @
  // VGA). We could do something useful, or we could just take a nice nap
  while (1) 
    __wfi();
  __builtin_unreachable();
}

#ifdef EAR_PIN
#define CPU_STEP_LOOP 10
#else
#define CPU_STEP_LOOP 100
#endif

void __not_in_flash_func(main_loop)() {
  
  unsigned int lastInterruptFrame = _frames;
  
  uint frames = 0;
  
  while (1) {
    tuh_task();
#ifdef USE_PS2_KBD
    ps2kbd.tick();
#endif
#ifdef USE_KEY_MATRIX
    hid_keyboard_report_t const *curr;
    hid_keyboard_report_t const *prev;
    zx_keyscan_get_hid_reports(&curr, &prev);
    process_picomputer_kbd_report(curr, prev);
#endif
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
#ifdef USE_PS2_KBD
  ps2kbd.init_gpio();
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

  // Configure the GPIO pins for audio
  zxSpectrumAudioInit();
 
  screenPtr = zxSpectrum.screenPtr();
  attrPtr = screenPtr + (32 * 24 * 8);

  keyboard1.setZxSpectrum(&zxSpectrum);
  keyboard2.setZxSpectrum(&zxSpectrum);

  // Initialise the menu renderer
  pcw_init_renderer();
  
#ifdef USE_KEY_MATRIX
  // Initialise the keyboard scan
  zx_keyscan_init();
#endif
  printf("Configuring DVI\n");
  dvi0.timing = &DVI_TIMING;
  dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
  dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

  // HDMI Audio related
  dvi_get_blank_settings(&dvi0)->top    = 0;
  dvi_get_blank_settings(&dvi0)->bottom = 0;
  dvi_audio_sample_buffer_set(&dvi0, audio_buffer, AUDIO_BUFFER_SIZE);
  dvi_set_audio_freq(&dvi0, 44100, 28000, 6272);
  add_repeating_timer_ms(2, audio_timer_callback, NULL, &audio_timer);

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

    // Create folders on the SD card if they are missing etc.
    picoRootWin.initialise();

    // Load quick save slot 1 if present
    quickSave.load(&zxSpectrum, 0);
    
    bool isKiosk = zxSpectrumKisok.isKiosk();
    keyboard1.setKiosk(isKiosk);
    keyboard2.setKiosk(isKiosk);
  }

  showMenu = false;
  picoRootWin.removeMessage();

  main_loop();
  
  __builtin_unreachable();
}
