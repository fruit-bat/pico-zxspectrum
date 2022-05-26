/*

This file should be tailored to match the hardware design.

There should be one element of the spi[] array for each hardware SPI used.

There should be one element of the sd_cards[] array for each SD card slot.
The name is should correspond to the FatFs "logical drive" identifier.
(See http://elm-chan.org/fsw/ff/doc/filename.html#vol)
The rest of the constants will depend on the type of
socket, which SPI it is driven by, and how it is wired.

*/

#include <string.h>
#include "my_debug.h"
#include "hw_config.h"
#include "ff.h" /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */

void __not_in_flash_func(spi1_dma_isr)();

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave
// selects.
static spi_t spis[] = {  // One for each SPI.
    {
        .hw_inst = spi1,  // SPI component
        .miso_gpio = 12, // GPIO number (not pin number)
        .mosi_gpio = 11,
        .sck_gpio = 10,
        /* The choice of SD card matters! SanDisk runs at the highest speed. PNY
           can only mangage 5 MHz. Those are all I've tried. */
        //.baud_rate = 1000 * 1000,
        .baud_rate = 12500 * 1000,  // The limitation here is SPI slew rate.
        //.baud_rate = 25 * 1000 * 1000, // Actual frequency: 20833333. Has
        // worked for me with SanDisk.

        // Following attributes are dynamically assigned
        .dma_isr = spi1_dma_isr,
        .initialized = false,  // initialized flag
    }
};

// Hardware Configuration of the SD Card "objects"
static sd_card_t sd_cards[] = {  // One for each SD card
    {
        .pcName = "0:",           // Name used to mount device
        .spi = &spis[0],          // Pointer to the SPI driving this card
        .ss_gpio = 13,            // The SPI slave select GPIO for this SD card
        .use_card_detect = false,
        // Following attributes are dynamically assigned
        .m_Status = STA_NOINIT,
        .sectors = 0,
        .card_type = 0,
    }
};

void spi1_dma_isr() { spi_irq_handler(&spis[0]); }

/* ********************************************************************** */
size_t __not_in_flash_func(sd_get_num)() { return count_of(sd_cards); }

sd_card_t *__not_in_flash_func(sd_get_by_num)(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_cards[num];
    } else {
        return NULL;
    }
}

size_t __not_in_flash_func(spi_get_num)() { return count_of(spis); }

spi_t *__not_in_flash_func(spi_get_by_num)(size_t num) {
    if (num <= sd_get_num()) {
        return &spis[num];
    } else {
        return NULL;
    }
}

/* [] END OF FILE */
