#pragma once

#include "hardware/gpio.h"
#include "hardware/adc.h"

static bool voltage_sensor_ready = false;

void init_voltage_sensor() {
    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);
    voltage_sensor_ready = true;
}

float read_voltage_sensor() {
    if (!voltage_sensor_ready) init_voltage_sensor();
    const float conversion_factor = (3.3f * 3.0f) / (1 << 12);
    uint16_t result = adc_read();
    const float v1 = (float)result * conversion_factor;

    return v1;
}
