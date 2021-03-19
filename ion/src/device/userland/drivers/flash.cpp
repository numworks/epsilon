#include <shared/drivers/flash_privileged.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Flash {

void MassErase() {
  SVC(SVC_FLASH_MASS_ERASE);
}

void SVC_ATTRIBUTES eraseSectorSVC(int * i) {
  SVC(SVC_FLASH_ERASE_SECTOR);
}

void EraseSector(int i) {
  eraseSectorSVC(&i);
}

void SVC_ATTRIBUTES writeMemorySVC(uint8_t * destination, uint8_t * source, size_t * length) {
  SVC(SVC_FLASH_WRITE_MEMORY);
}

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length) {
  writeMemorySVC(destination, source, &length);
}

}
}
}
