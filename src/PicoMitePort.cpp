#include "PicoMitePort.h"
#include "hardware/adc.h"

// Output pins on GP10, GP11, GP12, GP13
static const uint8_t POUT_PINS[] = {10, 11, 12, 13};

void PicoMitePort::init() {
    adc_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);

    // Select ADC input 2 (GP28)
    adc_select_input(2);

    for(size_t i = 0; i < sizeof(POUT_PINS); ++i) {
        gpio_init(POUT_PINS[i]);
        gpio_set_dir(POUT_PINS[i], GPIO_OUT);
    }
}

uint8_t __not_in_flash_func(PicoMitePort::read)() {
    uint16_t result = adc_read(); // Read 12 bit ADC
    return (result >> 4) & 0xff;
}

void __not_in_flash_func(PicoMitePort::write)(uint8_t data) {
    for(size_t i = 0; i < sizeof(POUT_PINS); ++i) {
        gpio_put(POUT_PINS[i], (data & 1) == 1);
        data >>= 1;
    }
}
