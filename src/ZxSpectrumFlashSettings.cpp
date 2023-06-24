#include "ZxSpectrumFlashSettings.h"
#include <hardware/flash.h>
#include <hardware/sync.h>
#include <cstring>

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

ZxSpectrumFlashSettings::ZxSpectrumFlashSettings()
{
}

// https://www.makermatrix.com/blog/read-and-write-data-with-the-pi-pico-onboard-flash/
// https://github.com/raspberrypi/pico-examples/issues/34
// https://kevinboone.me/picoflash.html?i=1

bool ZxSpectrumFlashSettings::onSave(ZxSpectrumSettingValues *values) {

    // Disable interrupts while we write flash
    uint32_t ints = save_and_disable_interrupts();

    // Erase the last sector of FLASH
    // TODO This crashes :-(
    // TODO Probably need to disable interrupts/execution on the other core?
    // TODO Possibly this is due to execution from flash on the other core?
    flash_range_erase (FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    // Write the setting to FLASH
    flash_range_program (FLASH_TARGET_OFFSET, (const uint8_t*)values, sizeof(ZxSpectrumSettingValues));

    // Restore interrupts
    restore_interrupts (ints);

    return true;
}

bool ZxSpectrumFlashSettings::onLoad(ZxSpectrumSettingValues *values) {

    memcpy(values, (const void *)(XIP_BASE + FLASH_TARGET_OFFSET), sizeof(ZxSpectrumSettingValues));

    return true;
}
