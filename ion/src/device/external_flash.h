#ifndef ION_DEVICE_EXTERNAL_FLASH_H
#define ION_DEVICE_EXTERNAL_FLASH_H

// Quad-SPI on STM32 microcontroller
// https://www.st.com/resource/en/application_note/dm00227538.pdf

// Adesto Technologies AT25SF641
// https://www.adestotech.com/wp-content/uploads/AT25SF641_111.pdf

/* External Flash Memory map             Address space
 *                       8MiB chip     0x000000 - 0x7FFFFF
 *   2^7                64KiB blocks   0x..0000 - 0x..FFFF
 *   2^7 * 2            32KiB blocks   0x..0000 - 0x..7FFF or 0x..8000 - 0x..FFFF
 *   2^7 * 2 * 2^3       4KiB blocks   0x...000 - 0x...FFF
 *   2^7 * 2 * 2^3 * 2^4 256B pages    0x....00 - 0x....FF */

namespace Ion {
namespace ExternalFlash {
namespace Device {

// Read, Erase, Write
// at the fastest possible speed

void eraseChip(); //block?

void program(uint32_t * source, uint32_t * destination, size_t length);

void read(); // in indirect read mode

// memory-mapped mode

}
}
}

#endif
