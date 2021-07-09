#include "external_flash_command.h"
#include <drivers/config/clocks.h>
#include <drivers/config/external_flash.h>
#include <drivers/external_flash.h>
#include <ion/timing.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

using namespace Regs;

static void initGPIO() {
  for(const AFGPIOPin & p : Config::Pins) {
    p.init();
  }
}

static void initQSPI() {
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
  /* Reset the chip, in case we jumped to start and the chip is already
   * initialized (otherwise the chip might for instance be in QPI, and not
   * understand SPI/QSPI instructions). As we don't know the current chip mode,
   * we need to send the instructions on 1 wire and on 4 wires.*/
  send_reset_commands();
  send_reset_commands(sOperatingModes400);

  // Release sleep deep
  send_command(Command::ReleaseDeepPowerDown);
  Timing::usleep(3);

  /* The chip initially expects commands in SPI mode. We need to use SPI to tell
   * it to switch to QuadSPI/QPI. */
  send_command(Command::WriteEnable);
  ExternalFlashStatusRegister::StatusRegister2 statusRegister2(0);
  statusRegister2.setQE(true);
  wait();
  send_write_command(Command::WriteStatusRegister2, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(&statusRegister2), sizeof(statusRegister2), sOperatingModes101);
  wait();
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
  send_reset_commands();

  // Sleep deep
  send_command(Command::DeepPowerDown);
  Timing::usleep(3);
}

static void shutdownQSPI() {
  // Reset the controller
  RCC.AHB3RSTR()->setQSPIRST(true);
  RCC.AHB3RSTR()->setQSPIRST(false);
}

void shutdown() {
  if (Config::NumberOfSectors == 0) {
    return;
  }
  shutdownChip();
  shutdownQSPI();
  shutdownGPIO();
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
    send_write_command(Command::Erase4KbyteBlock, reinterpret_cast<uint8_t *>(i << Config::NumberOfAddressBitsIn4KbyteBlock), nullptr, 0, sOperatingModes110);
  } else if (i < Config::NumberOf4KSectors + Config::NumberOf32KSectors) {
    /* If the sector is the number Config::NumberOf4KSectors, we want to write
     * at the address 1 << NumberOfAddressBitsIn32KbyteBlock, hence the formula
     * (i - Config::NumberOf4KSectors + 1). */
    send_write_command(Command::Erase32KbyteBlock, reinterpret_cast<uint8_t *>((i - Config::NumberOf4KSectors + 1) << Config::NumberOfAddressBitsIn32KbyteBlock), nullptr, 0, sOperatingModes110);
  } else {
    /* If the sector is the number
     * Config::NumberOf4KSectors - Config::NumberOf32KSectors, we want to write
     * at the address 1 << NumberOfAddressBitsIn32KbyteBlock, hence the formula
     * (i - Config::NumberOf4KSectors - Config::NumberOf32KSectors + 1). */
    send_write_command(Command::Erase64KbyteBlock, reinterpret_cast<uint8_t *>((i - Config::NumberOf4KSectors - Config::NumberOf32KSectors + 1) << Config::NumberOfAddressBitsIn64KbyteBlock), nullptr, 0, sOperatingModes110);
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

}
}
}

