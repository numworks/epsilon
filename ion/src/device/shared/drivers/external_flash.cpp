#include "external_flash.h"
#include <drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

/* The external flash and the Quad-SPI peripheral support
 * several operating modes, corresponding to different numbers of signals
 * used to communicate during each phase of the command sequence.
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
 * The external flash supports clock frequencies up to 104MHz for all instructions,
 * except for Read Data (0x03) which is supported up to 50Mhz.
 *
 *
 *                Quad-SPI block diagram
 *
 *               +----------------------+          +------------+
 *               |       Quad-SPI       |          |            |
 *               |      peripheral      |          |  External  |
 *               |                      |   read   |   flash    |
 *   AHB    <--  |   data   <-- 32-byte |   <--    |   memory   |
 *  matrix  -->  | regsiter -->  FIFO   |    -->   |            |
 *               +----------------------+   write  +------------+
 *
 * Any data transmitted to or from the external flash memory go through a 32-byte FIFO.
 *
 * Read or write operations are performed in burst mode, that is,
 * after any data byte is transmitted between the Quad-SPI and the flash memory,
 * the latter automatically increments the specified address and
 * the next byte to read or write is respectively pushed in or popped from the FIFO.
 * and so on, as long as the clock continues.
 *
 * If the FIFO gets full in a read operation or
 * if the FIFO gets empty in a write operation,
 * the operation stalls and CLK stays low until firmware services the FIFO.
 *
 * If the FIFO gets full in a write operation,
 * the operation is stalled until the FIFO has enough space to accept the amount of data being written.
 * If the FIFO does not have as many bytes as requested by the read operation and if BUSY=1,
 * the operation is stalled until enough data is present or until the transfer is complete, whichever happens first. */

class ExternalFlashStatusRegister {
public:
  class StatusRegister1 : Register8 {
  public:
    using Register8::Register8;
    REGS_BOOL_FIELD_R(BUSY, 0);
  };
  class StatusRegister2 : Register8 {
  public:
    using Register8::Register8;
    REGS_BOOL_FIELD_W(QE, 1);
  };
};

static constexpr QUADSPI::CCR::OperatingMode DefaultOperatingMode = QUADSPI::CCR::OperatingMode::Quad;
static constexpr int ClockFrequencyDivisor = 2;
static constexpr int ChipSelectHighTime = (ClockFrequencyDivisor == 1) ? 3 : (ClockFrequencyDivisor == 2) ? 2 : 1;

static void send_command_full(QUADSPI::CCR::FunctionalMode functionalMode, QUADSPI::CCR::OperatingMode operatingMode, Command c, uint8_t * address, uint32_t altBytes, size_t numberOfAltBytes, uint8_t dummyCycles, uint8_t * data, size_t dataLength);

static inline void send_command(Command c, QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectWrite,
    operatingMode,
    c,
    reinterpret_cast<uint8_t *>(FlashAddressSpaceSize),
    0, 0,
    0,
    nullptr, 0
  );
}

static inline void send_write_command(Command c, uint8_t * address, uint8_t * data, size_t dataLength, QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectWrite,
    operatingMode,
    c,
    address,
    0, 0,
    0,
    data, dataLength
  );
}

static inline void send_read_command(Command c, uint8_t * address, uint8_t * data, size_t dataLength, QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectRead,
    operatingMode,
    c,
    address,
    0, 0,
    0,
    data, dataLength
  );
}

static inline void wait(QUADSPI::CCR::OperatingMode operatingMode = DefaultOperatingMode) {
  ExternalFlashStatusRegister::StatusRegister1 statusRegister1(0);
  do {
    send_read_command(Command::ReadStatusRegister, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&statusRegister1), sizeof(statusRegister1), operatingMode);
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
  constexpr int FastReadDummyCycles = (DefaultOperatingMode == QUADSPI::CCR::OperatingMode::Single) ? 8 : (ClockFrequencyDivisor > 1) ? 4 : 6;
  send_command_full(
    QUADSPI::CCR::FunctionalMode::MemoryMapped,
    DefaultOperatingMode,
    Command::FastReadQuadIO,
    reinterpret_cast<uint8_t *>(FlashAddressSpaceSize),
    0xA0, 1,
    2, //FIXME
    nullptr, 0
  );
}

static void unset_memory_mapped_mode() {
  /* Reset Continuous Read Mode Bits before issuing normal instructions. */
  uint8_t dummyData;
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectRead,
    DefaultOperatingMode,
    Command::FastReadQuadIO,
    0,
    ~(0xA0), 1,
    2, //FIXME
    &dummyData, 1
  );
}

void send_command_full(QUADSPI::CCR::FunctionalMode functionalMode, QUADSPI::CCR::OperatingMode operatingMode, Command c, uint8_t * address, uint32_t altBytes, size_t numberOfAltBytes, uint8_t dummyCycles, uint8_t * data, size_t dataLength) {
  class QUADSPI::CCR ccr(0);
  ccr.setFMODE(functionalMode);
  if (data != nullptr || functionalMode == QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    ccr.setDMODE(operatingMode);
  }
  if (functionalMode != QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    QUADSPI.DLR()->set((dataLength > 0) ? dataLength-1 : 0);
  }
  ccr.setDCYC(dummyCycles);
  if (numberOfAltBytes > 0) {
    ccr.setABMODE(operatingMode);
    ccr.setABSIZE(static_cast<QUADSPI::CCR::Size>(numberOfAltBytes - 1));
    QUADSPI.ABR()->set(altBytes);
  }
  if (address != reinterpret_cast<uint8_t *>(FlashAddressSpaceSize) || functionalMode == QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    ccr.setADMODE(operatingMode);
    ccr.setADSIZE(QUADSPI::CCR::Size::ThreeBytes);
  }
  ccr.setIMODE(operatingMode);
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
   * peripheral is disabled." */
  if (functionalMode != QUADSPI::CCR::FunctionalMode::MemoryMapped) {
    while (QUADSPI.SR()->getBUSY()) {
    }
  }
}

void init() {
  initGPIO();
  initQSPI();
  initChip();
}

void initGPIO() {
  for(const AFGPIOPin & p : Config::Pins) {
    p.init();
  }
}

void initQSPI() {
  // Enable QUADSPI AHB3 peripheral clock
  RCC.AHB3ENR()->setQSPIEN(true);
  // Configure controller for target device
  class QUADSPI::DCR dcr(0);
  dcr.setFSIZE(NumberOfAddressBitsInChip - 1);
  dcr.setCSHT(ChipSelectHighTime - 1);
  dcr.setCKMODE(true);
  QUADSPI.DCR()->set(dcr);
  class QUADSPI::CR cr(0);
  cr.setPRESCALER(ClockFrequencyDivisor - 1);
  cr.setEN(true);
  QUADSPI.CR()->set(cr);
}

void initChip() {
  /* The chip initially expects commands in SPI mode. We need to use SPI to tell
   * it to switch to QPI. */
  if (DefaultOperatingMode == QUADSPI::CCR::OperatingMode::Quad) {
    send_command(Command::WriteEnable, QUADSPI::CCR::OperatingMode::Single);
    ExternalFlashStatusRegister::StatusRegister2 statusRegister2(0);
    statusRegister2.setQE(true);
    wait(QUADSPI::CCR::OperatingMode::Single);
    send_write_command(Command::WriteStatusRegister2, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&statusRegister2), sizeof(statusRegister2), QUADSPI::CCR::OperatingMode::Single);
    wait(QUADSPI::CCR::OperatingMode::Single);
    send_command(Command::EnableQPI, QUADSPI::CCR::OperatingMode::Single);
    wait();
    if (ClockFrequencyDivisor == 1) {
      class ReadParameters : Register8 {
      public:
        /* Parameters sent along with SetReadParameters instruction in order
         * to configure the number of dummy cycles for the QPI Read instructions. */
        using Register8::Register8;
        REGS_BOOL_FIELD_W(P5, 1);
      };
      ReadParameters readParameters(0);
      readParameters.setP5(true);
      send_write_command(Command::SetReadParameters, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&readParameters), sizeof(readParameters));
    }
  }
  set_as_memory_mapped();
}

int SectorAtAddress(uint32_t address) {
  int i = address >> NumberOfAddressBitsIn64KbyteBlock;
  if (i >= NumberOfSectors) {
    return -1;
  }
  return i;
}

void MassErase() {
  unset_memory_mapped_mode();
  send_command(Command::WriteEnable);
  wait();
  send_command(Command::ChipErase);
  wait();
  set_as_memory_mapped();
}

void EraseSector(int i) {
  assert(i >= 0 && i < NumberOfSectors);
  unset_memory_mapped_mode();
  send_command(Command::WriteEnable);
  wait();
  send_write_command(Command::Erase64KbyteBlock, reinterpret_cast<uint8_t *>(i << NumberOfAddressBitsIn64KbyteBlock), nullptr, 0);
  wait();
  set_as_memory_mapped();
}

void WriteMemory(uint8_t * source, uint8_t * destination, size_t length) {
  unset_memory_mapped_mode();
  /* Each 256-byte page of the external flash memory (contained in a previously erased area)
   * may be programmed in burst mode with a single Page Program instruction.
   * However, when the end of a page is reached, the addressing wraps to the beginning.
   * Hence a Page Program instruction must be issued for each page. */
  static constexpr size_t PageSize = 256;
  constexpr Command pageProgram = (DefaultOperatingMode == QUADSPI::CCR::OperatingMode::Single) ? Command::PageProgram : Command::QuadPageProgram;
  uint8_t offset = reinterpret_cast<uint32_t>(destination) & (PageSize - 1);
  size_t lengthThatFitsInPage = PageSize - offset;
  while (length > 0) {
    if (lengthThatFitsInPage > length) {
      lengthThatFitsInPage = length;
    }
    send_command(Command::WriteEnable);
    wait();
    send_write_command(pageProgram, destination, source, lengthThatFitsInPage);
    length -= lengthThatFitsInPage;
    destination += lengthThatFitsInPage;
    source += lengthThatFitsInPage;
    lengthThatFitsInPage = PageSize;
    wait();
  }
  set_as_memory_mapped();
}

}
}
}
