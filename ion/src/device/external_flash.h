#ifndef ION_DEVICE_EXTERNAL_FLASH_H
#define ION_DEVICE_EXTERNAL_FLASH_H

#include <stdint.h>
#include <stddef.h>
#include "regs/regs.h"

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

/*  Pin | Role                 | Mode                  | Function
 * -----+----------------------+-----------------------+-----------------
 *  PB2 | QUADSPI CLK          | Alternate Function  9 | QUADSPI_CLK
 *  PB6 | QUADSPI BK1_NCS      | Alternate Function 10 | QUADSPI_BK1_NCS
 *  PC8 | QUADSPI BK1_IO2/WP   | Alternate Function  9 | QUADSPI_BK1_IO2
 *  PC9 | QUADSPI BK1_IO0/SO   | Alternate Function  9 | QUADSPI_BK1_IO0
 * PD12 | QUADSPI BK1_IO1/SI   | Alternate Function  9 | QUADSPI_BK1_IO1
 * PD13 | QUADSPI BK1_IO3/HOLD | Alternate Function  9 | QUADSPI_BK1_IO3
 */

void init();
void shutdown();

void initGPIO();
void initQSPI();
void initChip();

void MassErase();
void EraseSector(int sector);
void WriteMemory(uint8_t * source, uint8_t * destination, size_t length);

enum class Command : uint8_t {
  ReadStatusRegister = 0x05,
  WriteStatusRegister2 = 0x31,
  WriteEnable = 0x06,
  ReadData = 0x03,
  FastRead = 0x0B,
  QuadPageProgram = 0x33,
  EnableQPI = 0x38,
  ChipErase = 0xC7
};

constexpr static uint32_t QSPIBaseAddress = 0x90000000;
constexpr static uint8_t FlashNumberOfAddressBits = 23;
constexpr static uint32_t FlashAddressSpaceSize = 1 << FlashNumberOfAddressBits;

constexpr static GPIOPin QSPIPins[] = {
  GPIOPin(GPIOB, 2), GPIOPin(GPIOB, 6), GPIOPin(GPIOC, 9), GPIOPin(GPIOD,12),
  GPIOPin(GPIOC, 8), GPIOPin(GPIOD,13)
};

}
}
}

#endif
