#include <shared/drivers/external_flash.h>
#include <shared/drivers/config/board.h>
#include <shared/drivers/trampoline.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

uint32_t addressTrampolineFunction(int index) {
  return Board::Config::BootloaderTrampolineAddress + sizeof(void *) * index;
}

typedef void (*MassEraseFunction)();

void TRAMPOLINE_ATTRIBUTES MassErase() {
  MassEraseFunction * trampolineFunction = reinterpret_cast<MassEraseFunction *>(addressTrampolineFunction(TRAMPOLINE_EXTERNAL_FLASH_MASS_ERASE));
  (*trampolineFunction)();
}

typedef void (*EraseSectorFunction)(int);

void TRAMPOLINE_ATTRIBUTES EraseSector(int i) {
   EraseSectorFunction * trampolineFunction = reinterpret_cast<EraseSectorFunction *>(addressTrampolineFunction(TRAMPOLINE_EXTERNAL_FLASH_ERASE_SECTOR));
  (*trampolineFunction)(i);
}

typedef void (*WriteMemoryFunction)(uint8_t *, const uint8_t *, size_t);

void TRAMPOLINE_ATTRIBUTES WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
   WriteMemoryFunction * trampolineFunction = reinterpret_cast<WriteMemoryFunction *>(addressTrampolineFunction(TRAMPOLINE_EXTERNAL_FLASH_WRITE_MEMORY));
  (*trampolineFunction)(destination, source, length);
}

}
}
}
