#ifndef _CUSTOM_DVI_PIN_CONFIGS_H
#define _CUSTOM_DVI_PIN_CONFIGS_H

// This file defines the TMDS pair layouts on a handful of boards I have been
// developing on. It's not a particularly important file -- just saves some
// copy + paste.

#include "dvi_serialiser.h"

#ifndef DVI_DEFAULT_SERIAL_CONFIG
#define DVI_DEFAULT_SERIAL_CONFIG pico_sock_cfg
#endif

#ifndef DVI_DEFAULT_PIO_INST
#define DVI_DEFAULT_PIO_INST pio0
#endif

// ----------------------------------------------------------------------------
//  Waveshare - RP2040 - PiZero

static const struct dvi_serialiser_cfg waveshare_rp2040_pizero_hdmi_cfg = {
	.pio = DVI_DEFAULT_PIO_INST,
	.sm_tmds = {0, 1, 2},
	.pins_tmds = {26, 24, 22},
	.pins_clk = 28,
	.invert_diffpairs = false
};

static const struct dvi_serialiser_cfg murmulator2_hdmi_cfg = {
	.pio = DVI_DEFAULT_PIO_INST,
	.sm_tmds = {0, 1, 2},
	.pins_tmds = {14, 16, 18},
	.pins_clk = 12,
	.invert_diffpairs = true
};

static const struct dvi_serialiser_cfg SpotPear_hdmi_cfg = {
	.pio = DVI_DEFAULT_PIO_INST,
	.sm_tmds = {0, 1, 2},
	.pins_tmds = {10, 12, 14},
	.pins_clk = 8,
	.invert_diffpairs = true
};

#endif
