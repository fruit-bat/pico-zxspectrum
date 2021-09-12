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
extern "C" {
#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"
#include "tmds_encode.h"
}
#include "ZxSpectrum.h"

#include "bsp/board.h"
#include "tusb.h"
#include <pico/printf.h>

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define FONT_CHAR_WIDTH 8
#define FONT_CHAR_HEIGHT 8

// DVDD 1.2V (1.1V seems ok too)
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 240
#define VREG_VSEL VREG_VOLTAGE_1_20
#define DVI_TIMING dvi_timing_640x480p_60hz

// Should be 25 for pico ?
#define LED_PIN 25

struct dvi_inst dvi0;
struct semaphore dvi_start_sem;

unsigned char* screenPtr;
unsigned char* attrPtr;

extern "C" void hid_app_task();

// Screen handler
//
// 256×192 Pixels
// 32x24 Charaters
//
// 240-192 = 48 => 24 border rows top and bottom
// 320-256 = 64 => 64 border pixels left and right
//
static inline void prepare_scanline(uint y) {
	static uint8_t scanbuf[FRAME_WIDTH];
	
	// TODO Fetch the border color which can change row by row
	uint8_t borderColor = 0x00; // Black border initiallly
	uint8_t fgColor = 0xff; // TODO forground
	uint8_t bgColor = 0x02; // TODO background
	
	if (y < 24 || y >= (24+192)) {
//		memset(scanbuf, borderColor, FRAME_WIDTH);
		memset(scanbuf, 0, sizeof(scanbuf));
	}
	else {
		uint v = y - 24;
//		memset(scanbuf, borderColor, 64);
//		memset(scanbuf + FRAME_WIDTH - 64, borderColor, 64);
//		uint8_t *p = scanbuf + 64;
		uint8_t *p = scanbuf + 8;
		uint8_t *s = screenPtr + ((v & 0x7) << 8) + ((v & 0x38) << 2) + ((v & 0xc0) << 5);
		for(uint i = 0; i < 32; ++i) {
			p[i] = s[i];
		}
/*		for(uint i = 0; i < 32; ++i) {
			uint8_t b = *s++;
			for (uint j = 0; j < 8; ++j) {
				uint8_t c = b & (0x80 >> j) ? fgColor : bgColor;
				*p++ = c;
				*p++ = c;
			}
		}
*/
	}
	
	uint32_t *tmdsbuf;
	queue_remove_blocking(&dvi0.q_tmds_free, &tmdsbuf);
//	dvi_scanbuf_main_1bpp((const uint32_t*)scanbuf, tmdsbuf, FRAME_WIDTH);
	tmds_encode_1bpp((const uint32_t*)scanbuf, tmdsbuf, FRAME_WIDTH);
	queue_add_blocking(&dvi0.q_tmds_valid, &tmdsbuf);
}

void core1_scanline_callback() {
	static uint y = 1;
	prepare_scanline(y++);
	if (y >= FRAME_HEIGHT) y -= FRAME_HEIGHT;
}

void __not_in_flash("main") core1_main() {
	dvi_register_irqs_this_core(&dvi0, DMA_IRQ_1);
	sem_acquire_blocking(&dvi_start_sem);

	dvi_start(&dvi0);

	// The text display is completely IRQ driven (takes up around 30% of cycles @
	// VGA). We could do something useful, or we could just take a nice nap
	while (1) 
		__wfi();
	__builtin_unreachable();
}


static ZxSpectrum zxSpectrum;

extern "C" void spectrum_keyboard_handler(hid_keyboard_report_t const *report) {
//  sorcerer2HidKeyboard.processHidReport(report);
}

extern "C" int __not_in_flash("main") main() {
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
    
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	screenPtr = zxSpectrum.screenPtr();
	attrPtr = screenPtr + (32 * 24 * 8);

	printf("Configuring DVI\n");
   
	dvi0.timing = &DVI_TIMING;
	dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
	dvi0.scanline_callback = core1_scanline_callback;
	dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

	printf("Prepare first scanline\n");

	prepare_scanline(0);

	printf("Core 1 start\n");
	sem_init(&dvi_start_sem, 0, 1);
	hw_set_bits(&bus_ctrl_hw->priority, BUSCTRL_BUS_PRIORITY_PROC1_BITS);
	multicore_launch_core1(core1_main);


	sem_release(&dvi_start_sem);

	zxSpectrum.reset();

	while (1) {
		tuh_task();
		zxSpectrum.step();
		hid_app_task();
	}
	__builtin_unreachable();
}
