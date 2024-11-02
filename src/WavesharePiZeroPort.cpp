#include "WavesharePiZeroPort.h"

// Output pins on GP9, GP10, GP11, GP12
static const uint8_t POUT_PINS[] = {9, 10, 11, 12};

// Input pins on GP13, GP14, GP15
static const uint8_t PIN_PINS[] = {13, 14, 15};

void WavesharePiZeroPort::init() {
    for(size_t i = 0; i < sizeof(POUT_PINS); ++i) {
        const uint8_t pin = POUT_PINS[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
    }
    for(size_t i = 0; i < sizeof(PIN_PINS); ++i) {
        const uint8_t pin = PIN_PINS[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_set_pulls(pin, true, false);
    }    
}

uint8_t __not_in_flash_func(WavesharePiZeroPort::read)() {
    uint32_t data = 0;
    for(size_t i = 0; i < sizeof(PIN_PINS); ++i) {
        data |= gpio_get(PIN_PINS[sizeof(PIN_PINS) - i - 1]) ? 1 : 0;
        data <<= 1;
    }
    return (uint8_t)data;
}

void __not_in_flash_func(WavesharePiZeroPort::write)(uint8_t data) {
    for(size_t i = 0; i < sizeof(POUT_PINS); ++i) {
        gpio_put(POUT_PINS[i], (data & 1) == 1);
        data >>= 1;
    }
}
