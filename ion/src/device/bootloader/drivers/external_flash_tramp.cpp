#include <drivers/external_flash.h>
#include <drivers/cache.h>
#include <drivers/config/external_flash.h>
#include <drivers/config/clocks.h>
#include <drivers/trampoline.h>
#include <ion/timing.h>
#include <assert.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

using namespace Regs;

/* The external flash and the Quad-SPI peripheral support several operating
 * modes, corresponding to different numbers of signals used to communicate
 * during each phase of the command sequence.
 *
 *   Mode name for | Number of signals used during each phase:
 *  external flash | Instruction | Address | Alt. bytes | Data
 * ----------------+-------------+---------+------------+------
 * Standard    SPI |      1      |    1    |     1      |   1
 * Dual-Output SPI |      1      |    1    |     1      |   2
 * Dual-I/O    SPI |      1      |    2    |     2      |   2
 * Quad-Output SPI |      1      |    1    |     1      |   4
 * Quad-I/O    SPI |      1      |    4    |     4      |   4
 *             QPI |      4      |    4    |     4      |   4
 *
 * The external flash supports clock frequencies up to 104MHz for all
 * instructions, except for Read Data (0x03) which is supported up to 50Mhz.
 *
 *
 *                Quad-SPI block diagram
 *
 *               +----------------------+          +------------+
 *               |       Quad-SPI       |          |            |
 *               |      peripheral      |          |  External  |
 *               |                      |   read   |   flash    |
 *   AHB    <--  |   data   <-- 32-byte |   <--    |   memory   |
 *  matrix  -->  | register -->  FIFO   |    -->   |            |
 *               +----------------------+   write  +------------+
 *
 * Any data transmitted to or from the external flash memory go through a
 * 32-byte FIFO.
 *
 * Read or write operations are performed in burst mode, that is, after any data
 * byte is transmitted between the Quad-SPI and the flash memory, the latter
 * automatically increments the specified address and the next byte to read or
 * write is respectively pushed in or popped from the FIFO.
 * And so on, as long as the clock continues.
 *
 * If the FIFO gets full in a read operation or
 * if the FIFO gets empty in a write operation,
 * the operation stalls and CLK stays low until firmware services the FIFO.
 *
 * If the FIFO gets full in a write operation, the operation is stalled until
 * the FIFO has enough space to accept the amount of data being written.
 * If the FIFO does not have as many bytes as requested by the read operation
 * and if BUSY=1, the operation is stalled until enough data is present or until
 * the transfer is complete, whichever happens first. */

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

static constexpr uint8_t NumberOfAddressBitsIn64KbyteBlock = 16;
static constexpr uint8_t NumberOfAddressBitsIn32KbyteBlock = 15;
static constexpr uint8_t NumberOfAddressBitsIn4KbyteBlock = 12;

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

/* W25Q64JV does not implement QPI-4-4-4, so we always send the instructions on
 * one wire only.*/
static constexpr OperatingModes sOperatingModes100(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::NoData, QUADSPI::CCR::OperatingMode::NoData);
static constexpr OperatingModes sOperatingModes101(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::NoData, QUADSPI::CCR::OperatingMode::Single);
static constexpr OperatingModes sOperatingModes110(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::NoData);
static constexpr OperatingModes sOperatingModes111(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single);
static constexpr OperatingModes sOperatingModes114(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Quad);
static constexpr OperatingModes sOperatingModes144(QUADSPI::CCR::OperatingMode::Single, QUADSPI::CCR::OperatingMode::Quad, QUADSPI::CCR::OperatingMode::Quad);

// static QUADSPI::CCR::OperatingMode sOperatingMode = QUADSPI::CCR::OperatingMode::Single;

static constexpr int ClockFrequencyDivisor = 2; // F(QUADSPI) = F(AHB) / ClockFrequencyDivisor
static constexpr int FastReadQuadIODummyCycles = 4; // Must be 4 for W25Q64JV (Fig 24.A page 34) and for AT25F641 (table 7.19 page 28)
/* According to datasheets, the CS signal should stay high (deselect the device)
 * for t_SHSL = 50ns at least.
 * -> Max of 30ns (see AT25F641 Sections 8.7 and 8.8),
 *           10ns and 50ns (see W25Q64JV Section 9.6). */
static constexpr float ChipSelectHighTimeInNanoSeconds = 50.0f;

static void send_command_full(
    QUADSPI::CCR::FunctionalMode functionalMode,
    OperatingModes operatingModes,
    Command c,
    uint8_t * address,
    uint32_t altBytes,
    size_t numberOfAltBytes,
    uint8_t dummyCycles,
    uint8_t * data,
    size_t dataLength);

static inline void send_command(Command c) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectWrite,
    sOperatingModes100,
    c,
    reinterpret_cast<uint8_t *>(FlashAddressSpaceSize),
    0, 0,
    0,
    nullptr, 0);
}

static inline void send_write_command(Command c, uint8_t * address, const uint8_t * data, size_t dataLength, OperatingModes operatingModes) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectWrite,
    operatingModes,
    c,
    address,
    0, 0,
    0,
    const_cast<uint8_t *>(data), dataLength);
}

static inline void send_read_command(Command c, uint8_t * address, uint8_t * data, size_t dataLength) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectRead,
    sOperatingModes101,
    c,
    address,
    0, 0,
    0,
    data, dataLength);
}

static inline void wait() {
  /* The DSB instruction guarantees the completion of a write operation before
   * polling the status register. */
  Cache::dsb();
  ExternalFlashStatusRegister::StatusRegister1 statusRegister1(0);
  do {
    send_read_command(Command::ReadStatusRegister1, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&statusRegister1), sizeof(statusRegister1));
  } while (statusRegister1.getBUSY());
}

static void set_as_memory_mapped() {
  /* In memory-mapped mode, all AHB masters may access the external flash memory as an internal one:
   * the programmed instruction is sent automatically whenever an AHB master reads in the Quad-SPI flash bank area.
   * (The QUADSPI_DLR register has no meaning and any access to QUADSPI_DR returns zero.)
   *
   * To anticipate sequential reads, the nCS signal is maintained low so as to
   * keep the read operation active and prefetch the subsequent bytes in the FIFO.
   *
   * It goes low, only if the low-power timeout counter is enabled.
   * (Flash memories tend to consume more when nCS is held low.) */
  send_command_full(
    QUADSPI::CCR::FunctionalMode::MemoryMapped,
    sOperatingModes144,
    Command::FastReadQuadIO,
    reinterpret_cast<uint8_t *>(FlashAddressSpaceSize),
    0xA0, 1,
    FastReadQuadIODummyCycles,
    nullptr, 0
  );
}

static void unset_memory_mapped_mode() {
  /* Reset Continuous Read Mode Bits before issuing normal instructions. */
  uint8_t dummyData;
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectRead,
    sOperatingModes144,
    Command::FastReadQuadIO,
    0,
    ~(0xA0), 1,
    FastReadQuadIODummyCycles,
    &dummyData, 1
  );
}

static void send_command_full(QUADSPI::CCR::FunctionalMode functionalMode, OperatingModes operatingModes, Command c, uint8_t * address, uint32_t altBytes, size_t numberOfAltBytes, uint8_t dummyCycles, uint8_t * data, size_t dataLength) {
  /* According to ST's Errata Sheet ES0360, "Wrong data can be read in
   * memory-mapped after an indirect mode operation". This is the workaround. */
  if (functionalMode == QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    QUADSPI::CCR::FunctionalMode previousMode = QUADSPI.CCR()->getFMODE();
    if (previousMode == QUADSPI::CCR::FunctionalMode::IndirectWrite || previousMode == QUADSPI::CCR::FunctionalMode::IndirectRead) {
      // Reset the address register
      QUADSPI.AR()->set(0); // No write to DR should be done after this
      if (previousMode == QUADSPI::CCR::FunctionalMode::IndirectRead) {
        // Make an abort request to stop the reading and clear the busy bit
        QUADSPI.CR()->setABORT(true);
        while (QUADSPI.CR()->getABORT()) {
        }
      }
    }
  } else if (QUADSPI.CCR()->getFMODE() == QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    /* "BUSY goes high as soon as the first memory-mapped access occurs. Because
     * of the prefetch operations, BUSY does not fall until there is a timeout,
     * there is an abort, or the peripheral is disabled". (From the Reference
     * Manual)
     * If we are leaving memory-mapped mode, we send an abort to clear BUSY. */
    QUADSPI.CR()->setABORT(true);
    while (QUADSPI.CR()->getABORT()) {
    }
  }

  assert(QUADSPI.CCR()->getFMODE() != QUADSPI::CCR::FunctionalMode::MemoryMapped || QUADSPI.SR()->getBUSY() == 0);

  class QUADSPI::CCR ccr(0);
  ccr.setFMODE(functionalMode);
  if (data != nullptr || functionalMode == QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    ccr.setDMODE(operatingModes.dataOperatingMode());
  }
  if (functionalMode != QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    QUADSPI.DLR()->set((dataLength > 0) ? dataLength-1 : 0);
  }
  ccr.setDCYC(dummyCycles);
  if (numberOfAltBytes > 0) {
    ccr.setABMODE(operatingModes.addressOperatingMode()); // Seems to always be the same as address mode
    ccr.setABSIZE(static_cast<QUADSPI::CCR::Size>(numberOfAltBytes - 1));
    QUADSPI.ABR()->set(altBytes);
  }
  if (address != reinterpret_cast<uint8_t *>(FlashAddressSpaceSize) || functionalMode == QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    ccr.setADMODE(operatingModes.addressOperatingMode());
    ccr.setADSIZE(QUADSPI::CCR::Size::ThreeBytes);
  }
  ccr.setIMODE(operatingModes.instructionOperatingMode());
  ccr.setINSTRUCTION(static_cast<uint8_t>(c));
  if (functionalMode == QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    ccr.setSIOO(true);
    /* If the SIOO bit is set, the instruction is sent only for the first command following a write to QUADSPI_CCR.
     * Subsequent command sequences skip the instruction phase, until there is a write to QUADSPI_CCR. */
  }
  QUADSPI.CCR()->set(ccr);
  if (address != reinterpret_cast<uint8_t *>(FlashAddressSpaceSize)) {
    QUADSPI.AR()->set(reinterpret_cast<uint32_t>(address));
  }

  if (functionalMode == QUADSPI::CCR::FunctionalMode::IndirectWrite) {
    for (size_t i=0; i<dataLength; i++) {
      QUADSPI.DR()->set(data[i]);
    }
  } else if (functionalMode == QUADSPI::CCR::FunctionalMode::IndirectRead) {
    for (size_t i=0; i<dataLength; i++) {
      data[i] = QUADSPI.DR()->get();
    }
  }

  /* Wait for the command to be sent.
   * "When configured in memory-mapped mode, because of the prefetch operations,
   * BUSY does not fall until there is a timeout, there is an abort, or the
   * peripheral is disabled.", so we do not wait if the device is in
   * memory-mapped mode. */
  if (functionalMode != QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    while (QUADSPI.SR()->getBUSY()) {
    }
  }
}

void init() {
  assert(false);
}

void shutdown() {
  assert(false);
}

int SectorAtAddress(uint32_t address) {
  /* WARNING: this code assumes that the flash sectors are of increasing size:
   * first all 4K sectors, then all 32K sectors, and finally all 64K sectors. */
  int i = address >> NumberOfAddressBitsIn64KbyteBlock;
  if (i > Config::NumberOf64KSectors) {
    return -1;
  }
  if (i >= 1) {
    return Config::NumberOf4KSectors + Config::NumberOf32KSectors + i - 1;
  }
  i = address >> NumberOfAddressBitsIn32KbyteBlock;
  if (i >= 1) {
    i = Config::NumberOf4KSectors + i - 1;
    assert(i >= 0 && i <= Config::NumberOf32KSectors);
    return i;
  }
  i = address >> NumberOfAddressBitsIn4KbyteBlock;
  assert(i <= Config::NumberOf4KSectors);
  return i;
}

void unlockFlash() {
  // Warning: unset_memory_mapped_mode must be called before
  send_command(Command::WriteEnable);
  wait();
  ExternalFlashStatusRegister::StatusRegister1 statusRegister1(0);
  ExternalFlashStatusRegister::StatusRegister2 statusRegister2(0);
  ExternalFlashStatusRegister::StatusRegister2 currentStatusRegister2(0);
  send_read_command(Command::ReadStatusRegister2, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&currentStatusRegister2), sizeof(currentStatusRegister2));
  statusRegister2.setQE(currentStatusRegister2.getQE());

  uint8_t registers[] = {statusRegister1.get(), statusRegister2.get()};
  send_write_command(Command::WriteStatusRegister, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(registers), sizeof(registers), sOperatingModes101);
  wait();
}

void JDECid(uint8_t * manufacturerID, uint8_t * memoryType, uint8_t * capacityType) {
  unset_memory_mapped_mode();
  struct JEDECId {
    uint8_t manufacturerID;
    uint8_t memoryType;
    uint8_t capacityType;
  };
  JEDECId id;
  send_read_command(Command::ReadJEDECID, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&id), sizeof(id));
  *manufacturerID = id.manufacturerID;
  *memoryType = id.memoryType;
  *capacityType = id.capacityType;
  set_as_memory_mapped();
}

void MassErase() {
  // Mass erase is not enabled on kernel
  assert(false);
}

void WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  asm("cpsid if");
  (*reinterpret_cast<void(**)(uint8_t*, const uint8_t*, size_t)>(Ion::Device::Trampoline::address(Ion::Device::Trampoline::ExternalFlashWriteMemory)))(destination, source, length);
  asm("cpsie if");
}

void EraseSector(int i) {
  asm("cpsid if");
  (*reinterpret_cast<void(**)(int)>(Ion::Device::Trampoline::address(Ion::Device::Trampoline::ExternalFlashEraseSector)))(i);
}

}
}
}
