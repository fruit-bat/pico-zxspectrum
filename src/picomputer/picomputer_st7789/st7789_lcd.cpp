/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "st7789_lcd.h"
#include "st7789_lcd.pio.h"

#ifndef LCD_PIN_DIN
#define LCD_PIN_DIN 19
#endif
#ifndef LCD_PIN_CLK
#define LCD_PIN_CLK 18
#endif
#ifndef LCD_PIN_CS
#define LCD_PIN_CS 21
#endif
#ifndef LCD_PIN_DC
#define LCD_PIN_DC 16
#endif
#ifndef LCD_PIN_BL
#define LCD_PIN_BL 20
#endif

#define SERIAL_CLK_DIV (2.0f)

// Format: cmd length (including cmd byte), post delay in units of 5 ms, then cmd payload
// Note the delays have been shortened a little
static const uint8_t st7789_init_seq[] = {
        1, 20, 0x01,                        // Software reset
        1, 10, 0x11,                        // Exit sleep mode
        2, 2, 0x3a, 0x63,                   // Set colour mode to 12 bit (16 bit 0x55)
        2, 0, 0x36, 0x70,                   // Set MADCTL: row then column, refresh is bottom to top ????
        5, 0, 0x2a, 0x00, 0x00, 0x01, 0x40, // CASET: column addresses from 0 to 320
        5, 0, 0x2b, 0x00, 0x00, 0x00, 0xf0, // RASET: row addresses from 0 to 240
        1, 2, 0x21,                         // Inversion on, then 10 ms delay (supposedly a hack?)
        1, 2, 0x13,                         // Normal display on, then 10 ms delay
        1, 2, 0x29,                         // Main screen turn on, then wait 500 ms
        0                                   // Terminate list
};

static inline void lcd_set_dc_cs(bool dc, bool cs) {
    sleep_us(1);
    gpio_put_masked((1u << LCD_PIN_DC) | (1u << LCD_PIN_CS), !!dc << LCD_PIN_DC | !!cs << LCD_PIN_CS);
    sleep_us(1);
}

static inline void lcd_write_cmd(PIO pio, uint sm, const uint8_t *cmd, size_t count) {
    st7789_lcd_wait_idle(pio, sm);
    lcd_set_dc_cs(0, 0);
    st7789_lcd_put(pio, sm, *cmd++);
    if (count >= 2) {
        st7789_lcd_wait_idle(pio, sm);
        lcd_set_dc_cs(1, 0);
        for (size_t i = 0; i < count - 1; ++i)
            st7789_lcd_put(pio, sm, *cmd++);
    }
    st7789_lcd_wait_idle(pio, sm);
    lcd_set_dc_cs(1, 1);
}

static inline void lcd_init(PIO pio, uint sm, const uint8_t *init_seq) {
    const uint8_t *cmd = init_seq;
    while (*cmd) {
        lcd_write_cmd(pio, sm, cmd + 2, *cmd);
        sleep_ms(*(cmd + 1) * 5);
        cmd += *cmd + 2;
    }
}

static inline void st7789_start_pixels(PIO pio, uint sm) {
    uint8_t cmd = 0x2c; // RAMWR
    lcd_write_cmd(pio, sm, &cmd, 1);
    lcd_set_dc_cs(1, 0);
}

void st7789_init(PIO pio, uint sm) {
    
    uint offset = pio_add_program(pio, &st7789_lcd_program);
    st7789_lcd_program_init(pio, sm, offset, LCD_PIN_DIN, LCD_PIN_CLK, SERIAL_CLK_DIV, 8);

    gpio_init(LCD_PIN_CS);
    gpio_init(LCD_PIN_DC);
#ifndef LCD_NO_BL
    gpio_init(LCD_PIN_BL);
#endif
    gpio_set_dir(LCD_PIN_CS, GPIO_OUT);
    gpio_set_dir(LCD_PIN_DC, GPIO_OUT);
#ifndef LCD_NO_BL
    gpio_set_dir(LCD_PIN_BL, GPIO_OUT);
#endif
    gpio_put(LCD_PIN_CS, 1);
    lcd_init(pio, sm, st7789_init_seq);
#ifndef LCD_NO_BL
    gpio_put(LCD_PIN_BL, 1);
#endif

    st7789_start_pixels(pio, sm);
    st7789_lcd_wait_idle(pio, sm);
    st7789_lcd_program_init(pio, sm, offset, LCD_PIN_DIN, LCD_PIN_CLK, SERIAL_CLK_DIV, 24);
}

