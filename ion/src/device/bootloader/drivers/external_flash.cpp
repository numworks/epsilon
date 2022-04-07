#include <drivers/external_flash.h>
#include <drivers/cache.h>
#include <drivers/config/external_flash.h>
#include <drivers/config/clocks.h>
#include <ion/timing.h>

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
    REGS_BOOL_FIELD(BP, 2);
    REGS_BOOL_FIELD(BP1, 3);
    REGS_BOOL_FIELD(BP2, 4);
    REGS_BOOL_FIELD(TB, 5);
  };
  class StatusRegister2 : public Register8 {
  public:
    using Register8::Register8;
    REGS_BOOL_FIELD(SRP1, 0); 
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

static QUADSPI::CCR::OperatingMode sOperatingMode = QUADSPI::CCR::OperatingMode::Single;

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

static void initGPIO() {
  for(const AFGPIOPin & p : Config::Pins) {
    p.init();
  }
}

static void initQSPI() {
  // Enable QUADSPI AHB3 peripheral clock
  RCC.AHB3ENR()->setQSPIEN(true);

 // Configure controller for target device
  class QUADSPI::DCR dcr(0);
  dcr.setFSIZE(NumberOfAddressBitsInChip - 1);
  constexpr int ChipSelectHighTimeCycles = (ChipSelectHighTimeInNanoSeconds * static_cast<float>(Clocks::Config::AHBFrequency)) / (static_cast<float>(ClockFrequencyDivisor) * 1000.0f) + 1.0f;
  dcr.setCSHT(ChipSelectHighTimeCycles - 1);
  dcr.setCKMODE(true);
  QUADSPI.DCR()->set(dcr);
  class QUADSPI::CR cr(0);
  cr.setPRESCALER(ClockFrequencyDivisor - 1);
  cr.setEN(true);
  QUADSPI.CR()->set(cr);
}

static void initChip() {
  // Release sleep deep
  send_command(Command::ReleaseDeepPowerDown);
  Timing::usleep(3);

  /* The chip initially expects commands in SPI mode. We need to use SPI to tell
   * it to switch to QuadSPI/QPI. */
  if (sOperatingMode == QUADSPI::CCR::OperatingMode::Single) {
    send_command(Command::WriteEnable);
    ExternalFlashStatusRegister::StatusRegister2 statusRegister2(0);
    statusRegister2.setQE(true);
    wait();
    send_write_command(Command::WriteStatusRegister2, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&statusRegister2), sizeof(statusRegister2), sOperatingModes101);
    wait();
    sOperatingMode = QUADSPI::CCR::OperatingMode::Quad;
  }
  set_as_memory_mapped();
}

void init() {
  if (Config::NumberOfSectors == 0) {
    return;
  }
  initGPIO();
  initQSPI();
  initChip();
}

static void shutdownGPIO() {
  for(const AFGPIOPin & p : Config::Pins) {
    p.group().OSPEEDR()->setOutputSpeed(p.pin(), GPIO::OSPEEDR::OutputSpeed::Low);
    p.group().MODER()->setMode(p.pin(), GPIO::MODER::Mode::Analog);
    p.group().PUPDR()->setPull(p.pin(), GPIO::PUPDR::Pull::None);
  }
}

static void shutdownChip() {
  unset_memory_mapped_mode();
  // Reset
  send_command(Command::EnableReset);
  send_command(Command::Reset);
  sOperatingMode = QUADSPI::CCR::OperatingMode::Single;
  Timing::usleep(30);

  // Sleep deep
  send_command(Command::DeepPowerDown);
  Timing::usleep(3);
}

static void shutdownQSPI() {
  // Reset the controller
  RCC.AHB3RSTR()->setQSPIRST(true);
  RCC.AHB3RSTR()->setQSPIRST(false);

  RCC.AHB3ENR()->setQSPIEN(false); // TODO: move in Device::shutdownClocks
}

void shutdown() {
  if (Config::NumberOfSectors == 0) {
    return;
  }
  shutdownChip();
  shutdownQSPI();
  shutdownGPIO();
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
    assert(i >= Config::NumberOf4KSectors && i <= Config::NumberOf4KSectors + Config::NumberOf32KSectors);
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

void LockSlotA() {
  unset_memory_mapped_mode();
  unlockFlash();
  send_command(Command::WriteEnable);
  wait();
  ExternalFlashStatusRegister::StatusRegister1 statusRegister1(0);
  ExternalFlashStatusRegister::StatusRegister2 statusRegister2(0);
  ExternalFlashStatusRegister::StatusRegister2 currentStatusRegister2(0);
  send_read_command(Command::ReadStatusRegister2, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&currentStatusRegister2), sizeof(currentStatusRegister2));
  statusRegister2.setQE(currentStatusRegister2.getQE());
  statusRegister2.setSRP1(true);
  statusRegister1.setTB(true);
  statusRegister1.setBP2(true);
  statusRegister1.setBP1(true);
  uint8_t registers[] = {statusRegister1.get(), statusRegister2.get()};
  send_write_command(Command::WriteStatusRegister, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(registers), sizeof(registers), sOperatingModes101);
  wait();
  set_as_memory_mapped();
}

void LockSlotB() {
  unset_memory_mapped_mode();
  unlockFlash();
  send_command(Command::WriteEnable);
  wait();
  ExternalFlashStatusRegister::StatusRegister1 statusRegister1(0);
  ExternalFlashStatusRegister::StatusRegister2 statusRegister2(0);
  ExternalFlashStatusRegister::StatusRegister2 currentStatusRegister2(0);
  send_read_command(Command::ReadStatusRegister2, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&currentStatusRegister2), sizeof(currentStatusRegister2));
  statusRegister2.setQE(currentStatusRegister2.getQE());
  statusRegister2.setSRP1(true);
  statusRegister1.setTB(false);
  statusRegister1.setBP2(true);
  statusRegister1.setBP1(true);
  uint8_t registers[] = {statusRegister1.get(), statusRegister2.get()};
  send_write_command(Command::WriteStatusRegister, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(registers), sizeof(registers), sOperatingModes101);
  wait();
  set_as_memory_mapped();
}

void MassErase() {
  if (Config::NumberOfSectors == 0) {
    return;
  }
  unset_memory_mapped_mode();
  unlockFlash();
  send_command(Command::WriteEnable);
  wait();
  send_command(Command::ChipErase);
  wait();
  set_as_memory_mapped();
}

void __attribute__((noinline)) EraseSector(int i) {
  assert(i >= 0 && i < Config::NumberOfSectors);
  unset_memory_mapped_mode();
  unlockFlash();
  send_command(Command::WriteEnable);
  wait();
  /* WARNING: this code assumes that the flash sectors are of increasing size:
   * first all 4K sectors, then all 32K sectors, and finally all 64K sectors. */
  if (i < Config::NumberOf4KSectors) {
    send_write_command(Command::Erase4KbyteBlock, reinterpret_cast<uint8_t *>(i << NumberOfAddressBitsIn4KbyteBlock), nullptr, 0, sOperatingModes110);
  } else if (i < Config::NumberOf4KSectors + Config::NumberOf32KSectors) {
    /* If the sector is the number Config::NumberOf4KSectors, we want to write
     * at the address 1 << NumberOfAddressBitsIn32KbyteBlock, hence the formula
     * (i - Config::NumberOf4KSectors + 1). */
    send_write_command(Command::Erase32KbyteBlock, reinterpret_cast<uint8_t *>((i - Config::NumberOf4KSectors + 1) << NumberOfAddressBitsIn32KbyteBlock), nullptr, 0, sOperatingModes110);
  } else {
    /* If the sector is the number
     * Config::NumberOf4KSectors - Config::NumberOf32KSectors, we want to write
     * at the address 1 << NumberOfAddressBitsIn32KbyteBlock, hence the formula
     * (i - Config::NumberOf4KSectors - Config::NumberOf32KSectors + 1). */
    send_write_command(Command::Erase64KbyteBlock, reinterpret_cast<uint8_t *>((i - Config::NumberOf4KSectors - Config::NumberOf32KSectors + 1) << NumberOfAddressBitsIn64KbyteBlock), nullptr, 0, sOperatingModes110);
  }
  wait();
  set_as_memory_mapped();
}

void __attribute__((noinline)) WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  if (Config::NumberOfSectors == 0) {
    return;
  }
  destination -= ExternalFlash::Config::StartAddress;
  unset_memory_mapped_mode();
  /* Each 256-byte page of the external flash memory (contained in a previously erased area)
   * may be programmed in burst mode with a single Page Program instruction.
   * However, when the end of a page is reached, the addressing wraps to the beginning.
   * Hence a Page Program instruction must be issued for each page. */
  static constexpr size_t PageSize = 256;
  uint8_t offset = reinterpret_cast<uint32_t>(destination) & (PageSize - 1);
  size_t lengthThatFitsInPage = PageSize - offset;
  while (length > 0) {
    if (lengthThatFitsInPage > length) {
      lengthThatFitsInPage = length;
    }
    send_command(Command::WriteEnable);
    wait();

    /* Some chips implement 0x32 only, others 0x33 only, we call both. This does
     * not seem to affect the writing. */
    send_write_command(Command::QuadPageProgramAT25F641, destination, source, lengthThatFitsInPage, sOperatingModes144);
    send_write_command(Command::QuadPageProgramW25Q64JV, destination, source, lengthThatFitsInPage, sOperatingModes114);

    length -= lengthThatFitsInPage;
    destination += lengthThatFitsInPage;
    source += lengthThatFitsInPage;
    lengthThatFitsInPage = PageSize;
    wait();
  }
  set_as_memory_mapped();
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

}
}
}
