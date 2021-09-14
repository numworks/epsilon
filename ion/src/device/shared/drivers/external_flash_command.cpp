#include "external_flash_command.h"
#include <drivers/cache.h>
#include <config/clocks.h>
#include <config/external_flash.h>
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

void send_command(Command c, OperatingModes operatingModes) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectWrite,
    operatingModes,
    c,
    reinterpret_cast<uint8_t *>(FlashAddressSpaceSize),
    0, 0,
    0,
    nullptr, 0);
}

void send_reset_commands(OperatingModes operatingModes) {
  send_command(Command::EnableReset, operatingModes);
  send_command(Command::Reset, operatingModes);
  Timing::usleep(30);
}

void send_write_command(Command c, uint8_t * address, const uint8_t * data, size_t dataLength, OperatingModes operatingModes) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectWrite,
    operatingModes,
    c,
    address,
    0, 0,
    0,
    const_cast<uint8_t *>(data), dataLength);
}

void send_read_command(Command c, uint8_t * address, uint8_t * data, size_t dataLength) {
  send_command_full(
    QUADSPI::CCR::FunctionalMode::IndirectRead,
    sOperatingModes101,
    c,
    address,
    0, 0,
    0,
    data, dataLength);
}

void wait() {
  /* The DSB instruction guarantees the completion of a write operation before
   * polling the status register. */
  Cache::dsb();
  ExternalFlashStatusRegister::StatusRegister1 statusRegister1(0);
  do {
    send_read_command(Command::ReadStatusRegister1, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&statusRegister1), sizeof(statusRegister1));
  } while (statusRegister1.getBUSY());
}

void set_as_memory_mapped() {
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

void unset_memory_mapped_mode() {
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

}
}
}

