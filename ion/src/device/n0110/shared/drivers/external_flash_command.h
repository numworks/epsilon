#ifndef ION_DEVICE_N0110_SHARED_DRIVERS_EXTERNAL_FLASH_COMMAND_H
#define ION_DEVICE_N0110_SHARED_DRIVERS_EXTERNAL_FLASH_COMMAND_H

// Quad-SPI on STM32 microcontroller
// https://www.st.com/resource/en/application_note/dm00227538.pdf

// Adesto Technologies AT25SF641
// https://www.adestotech.com/wp-content/uploads/AT25SF641_111.pdf

/* External Flash Memory map             Address space
 *                       8MiB chip     0x000000 - 0x7FFFFF
 *   2^7                64KiB blocks   0x..0000 - 0x..FFFF
 *   2^7 * 2            32KiB blocks   0x..0000 - 0x..7FFF or 0x..8000 - 0x..FFFF
 *   2^7 * 2 * 2^3       4KiB blocks   0x...000 - 0x...FFF
 *   2^7 * 2 * 2^3 * 2^4 256B pages    0x....00 - 0x....FF
 *
 * To be able to erase a small sector for the exam mode, we say that the flash
 * is cut into 8 + 1 + 2^7-1 = 136 sectors: 8 sectors of 4Kb, 1 sector of 32Kb
 * and 2^7-1 sectors of 64Kb. These sectors are the smallest erasable units. If
 * need be, we can define more sectors to erase even more finely the flash. */

#include <drivers/config/clocks.h>
#include <regs/regs.h>
#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

static constexpr uint8_t NumberOfAddressBitsInChip = 23; // 2^23 Bytes = 8 MBytes
static constexpr uint32_t FlashAddressSpaceSize = 1 << NumberOfAddressBitsInChip;

enum class Command : uint8_t {
  ReadStatusRegister1 = 0x05,
  ReadStatusRegister2 = 0x35,
  WriteStatusRegister = 0x01,
  WriteStatusRegister2 = 0x31,
  WriteEnable = 0x06,
  ReadData = 0x03,
  FastRead = 0x0B,
  FastReadQuadIO = 0xEB,
  // Program previously erased memory areas as being "0"
  PageProgram = 0x02,
  QuadPageProgram = 0x33,
  EnableQPI = 0x38,
  EnableReset = 0x66,
  Reset = 0x99,
  // Erase the whole chip or a 64-Kbyte block as being "1"
  ChipErase = 0xC7,
  Erase4KbyteBlock = 0x20,
  Erase32KbyteBlock = 0x52,
  Erase64KbyteBlock = 0xD8,
  SetReadParameters = 0xC0,
  DeepPowerDown = 0xB9,
  ReleaseDeepPowerDown = 0xAB,
  ReadJEDECID = 0x9F
};

class ExternalFlashStatusRegister {
public:
  class StatusRegister1 : public Regs::Register8 {
  public:
    using Regs::Register8::Register8;
    REGS_BOOL_FIELD_R(BUSY, 0);
  };
  class StatusRegister2 : public Regs::Register8 {
  public:
    using Regs::Register8::Register8;
    REGS_BOOL_FIELD(QE, 1);
  };
};

static constexpr Regs::QUADSPI::CCR::OperatingMode DefaultOperatingMode = Regs::QUADSPI::CCR::OperatingMode::Quad;
static constexpr int ClockFrequencyDivisor = 2;
static constexpr bool ajustNumberOfDummyCycles = Clocks::Config::AHBFrequency > (80 * ClockFrequencyDivisor);
static constexpr int FastReadDummyCycles = (DefaultOperatingMode == Regs::QUADSPI::CCR::OperatingMode::Quad && ajustNumberOfDummyCycles) ? 4 : 2;

void send_command_full(Regs::QUADSPI::CCR::FunctionalMode functionalMode, Regs::QUADSPI::CCR::OperatingMode operatingMode, Command c, uint8_t * address, uint32_t altBytes, size_t numberOfAltBytes, uint8_t dummyCycles, uint8_t * data, size_t dataLength);

void send_command(Command c, Regs::QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode);

void send_write_command(Command c, uint8_t * address, const uint8_t * data, size_t dataLength, Regs::QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode);

void send_read_command(Command c, uint8_t * address, uint8_t * data, size_t dataLength, Regs::QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode);

void wait(Regs::QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode);

void set_as_memory_mapped();
void unset_memory_mapped_mode();

}
}
}

#endif
