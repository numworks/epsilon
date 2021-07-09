#ifndef ION_DEVICE_FLASHER_EXTERNAL_FLASH_COMMAND_H
#define ION_DEVICE_FLASHER_EXTERNAL_FLASH_COMMAND_H

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

using namespace Regs;

static constexpr uint8_t NumberOfAddressBitsInChip = 23; // 2^23 Bytes = 8 MBytes
static constexpr uint32_t FlashAddressSpaceSize = 1 << NumberOfAddressBitsInChip;

enum class Command : uint8_t {
  WriteStatusRegister     = 0x01,
  PageProgram             = 0x02, // Program previously erased memory areas as being "0"
  ReadData                = 0x03,
  ReadStatusRegister1     = 0x05,
  WriteEnable             = 0x06,
  Erase4KbyteBlock        = 0x20,
  WriteStatusRegister2    = 0x31,
  QuadPageProgramW25Q64JV = 0x32,
  QuadPageProgramAT25F641 = 0x33,
  ReadStatusRegister2     = 0x35,
  Erase32KbyteBlock       = 0x52,
  EnableReset             = 0x66,
  Reset                   = 0x99,
  ReadJEDECID             = 0x9F,
  ReleaseDeepPowerDown    = 0xAB,
  DeepPowerDown           = 0xB9,
  ChipErase               = 0xC7, // Erase the whole chip or a 64-Kbyte block as being "1"
  Erase64KbyteBlock       = 0xD8,
  FastReadQuadIO          = 0xEB
};

class ExternalFlashStatusRegister {
public:
  class StatusRegister1 : public Register8 {
  public:
    using Register8::Register8;
    REGS_BOOL_FIELD_R(BUSY, 0);
  };
  class StatusRegister2 : public Register8 {
  public:
    using Register8::Register8;
    REGS_BOOL_FIELD(QE, 1);
  };
};

class OperatingModes {
public:
  constexpr OperatingModes(
      QUADSPI::CCR::OperatingMode instruction,
      QUADSPI::CCR::OperatingMode address,
      QUADSPI::CCR::OperatingMode data) :
    m_instructionOperatingMode(instruction),
    m_addressOperatingMode(address),
    m_dataOperatingMode(data)
  {}
  QUADSPI::CCR::OperatingMode instructionOperatingMode() const { return m_instructionOperatingMode; }
  QUADSPI::CCR::OperatingMode addressOperatingMode() const { return m_addressOperatingMode; }
  QUADSPI::CCR::OperatingMode dataOperatingMode() const { return m_dataOperatingMode; }
private:
  QUADSPI::CCR::OperatingMode m_instructionOperatingMode;
  QUADSPI::CCR::OperatingMode m_addressOperatingMode;
  QUADSPI::CCR::OperatingMode m_dataOperatingMode;
};

/* W25Q64JV does not implement QPI-4-4-4, so we almost always send the
 * instructions on one wire only. The only exception is when sending the reset
 * command, see comment in initChip.*/
static constexpr OperatingModes sOperatingModes100(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::NoData, QUADSPI::CCR::OperatingMode::NoData);
static constexpr OperatingModes sOperatingModes101(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::NoData, QUADSPI::CCR::OperatingMode::Single);
static constexpr OperatingModes sOperatingModes110(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::NoData);
static constexpr OperatingModes sOperatingModes111(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single);
static constexpr OperatingModes sOperatingModes114(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Quad);
static constexpr OperatingModes sOperatingModes144(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Quad, QUADSPI::CCR::OperatingMode::Quad);
static constexpr OperatingModes sOperatingModes400(QUADSPI::CCR::OperatingMode::Quad, QUADSPI::CCR::OperatingMode::NoData, QUADSPI::CCR::OperatingMode::NoData);

static constexpr int ClockFrequencyDivisor = 2; // F(QUADSPI) = F(AHB) / ClockFrequencyDivisor
static constexpr int FastReadQuadIODummyCycles = 4; // Must be 4 for W25Q64JV (Fig 24.A page 34) and for AT25F641 (table 7.19 page 28)
/* According to datasheets, the CS signal should stay high (deselect the device)
 * for t_SHSL = 50ns at least.
 * -> Max of 30ns (see AT25F641 Sections 8.7 and 8.8),
 *           10ns and 50ns (see W25Q64JV Section 9.6). */
static constexpr float ChipSelectHighTimeInNanoSeconds = 50.0f;

void send_command(Command c, OperatingModes operatingModes = sOperatingModes100);
void send_reset_commands(OperatingModes operatingModes = sOperatingModes100);
void send_write_command(Command c, uint8_t * address, const uint8_t * data, size_t dataLength, OperatingModes operatingModes);
void send_read_command(Command c, uint8_t * address, uint8_t * data, size_t dataLength);
void wait();

void set_as_memory_mapped();
void unset_memory_mapped_mode();

}
}
}

#endif
