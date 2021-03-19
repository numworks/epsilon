#include <shared/drivers/flash_privileged.h>
#include <shared/drivers/external_flash_command.h>
#include <drivers/config/external_flash.h>

namespace Ion {
namespace Device {
namespace Flash {

using namespace Regs;
using namespace ExternalFlash;


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
  send_write_command(Command::WriteStatusRegister, reinterpret_cast<uint8_t *>(FlashAddressSpaceSize), reinterpret_cast<uint8_t *>(registers), sizeof(registers));
  wait();
}

void MassErase() {
  if (Config::NumberOfSectors == 0) {
    return;
  }
  if (MassEraseEnable()) {
    unset_memory_mapped_mode();
    unlockFlash();
    send_command(Command::WriteEnable);
    wait();
    send_command(Command::ChipErase);
    wait();
    set_as_memory_mapped();
  }
  for (int i = 0; i <= TotalNumberOfSectors(); i++) {
    if (!ForbiddenSector(i)) {
      EraseSector(i);
    }
  }
}

bool __attribute__((noinline)) EraseSector(int i) {
  assert(i >= 0 && i < Config::NumberOfSectors);
  if (ForbiddenSector(i)) {
    return false;
  }
  unset_memory_mapped_mode();
  unlockFlash();
  send_command(Command::WriteEnable);
  wait();
  /* WARNING: this code assumes that the flash sectors are of increasing size:
   * first all 4K sectors, then all 32K sectors, and finally all 64K sectors. */
  if (i < Config::NumberOf4KSectors) {
    send_write_command(Command::Erase4KbyteBlock, reinterpret_cast<uint8_t *>(i << Config::NumberOfAddressBitsIn4KbyteBlock), nullptr, 0);
  } else if (i < Config::NumberOf4KSectors + Config::NumberOf32KSectors) {
    /* If the sector is the number Config::NumberOf4KSectors, we want to write
     * at the address 1 << NumberOfAddressBitsIn32KbyteBlock, hence the formula
     * (i - Config::NumberOf4KSectors + 1). */
    send_write_command(Command::Erase32KbyteBlock, reinterpret_cast<uint8_t *>((i - Config::NumberOf4KSectors + 1) << Config::NumberOfAddressBitsIn32KbyteBlock), nullptr, 0);
  } else {
    /* If the sector is the number
     * Config::NumberOf4KSectors - Config::NumberOf32KSectors, we want to write
     * at the address 1 << NumberOfAddressBitsIn32KbyteBlock, hence the formula
     * (i - Config::NumberOf4KSectors - Config::NumberOf32KSectors + 1). */
    send_write_command(Command::Erase64KbyteBlock, reinterpret_cast<uint8_t *>((i - Config::NumberOf4KSectors - Config::NumberOf32KSectors + 1) << Config::NumberOfAddressBitsIn64KbyteBlock), nullptr, 0);
  }
  wait();
  set_as_memory_mapped();
  return true;
}

bool __attribute__((noinline)) WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  if (Config::NumberOfSectors == 0) {
    return false;
  }
  int firstSector = SectorAtAddress(reinterpret_cast<uint32_t >(source));
  int lastSector = SectorAtAddress(reinterpret_cast<uint32_t >(source + length - 1));
  for (int i = firstSector; i <= lastSector; i++) {
    if (ForbiddenSector(i)) {
      return false;
    }
  }
  destination -= ExternalFlash::Config::StartAddress;
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
  return true;
}

}
}
}
