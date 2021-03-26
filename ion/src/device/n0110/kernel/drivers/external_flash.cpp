#include <shared/drivers/external_flash.h>
#include <kernel/drivers/trampoline.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

typedef void (*MassEraseFunction)();

void TRAMPOLINE_ATTRIBUTES MassErase() {
  MassEraseFunction * trampolineFunction = reinterpret_cast<MassEraseFunction *>(Trampoline::addressOfFunction(TRAMPOLINE_EXTERNAL_FLASH_MASS_ERASE));
  (*trampolineFunction)();
}

typedef void (*EraseSectorFunction)(int);

void TRAMPOLINE_ATTRIBUTES EraseSector(int i) {
   EraseSectorFunction * trampolineFunction = reinterpret_cast<EraseSectorFunction *>(Trampoline::addressOfFunction(TRAMPOLINE_EXTERNAL_FLASH_ERASE_SECTOR));
  (*trampolineFunction)(i);
}

typedef void (*WriteMemoryFunction)(uint8_t *, const uint8_t *, size_t);

void TRAMPOLINE_ATTRIBUTES WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
   WriteMemoryFunction * trampolineFunction = reinterpret_cast<WriteMemoryFunction *>(Trampoline::addressOfFunction(TRAMPOLINE_EXTERNAL_FLASH_WRITE_MEMORY));
  (*trampolineFunction)(destination, source, length);
}

}
}
}
