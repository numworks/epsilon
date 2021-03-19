#include <shared/drivers/flash_privileged.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace Flash {

void MassErase() {
  SVC(SVC_FLASH_MASS_ERASE);
}

void SVC_ATTRIBUTES eraseSectorSVC(int * i, bool * res) {
  SVC(SVC_FLASH_ERASE_SECTOR);
}

bool EraseSector(int i) {
  bool res;
  eraseSectorSVC(&i, &res);
  return res;
}

void SVC_ATTRIBUTES writeMemorySVC(uint8_t * destination, const uint8_t * source, size_t * length, bool * res) {
  SVC(SVC_FLASH_WRITE_MEMORY);
}

bool WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  bool res;
  writeMemorySVC(destination, source, &length, &res);
  return res;
}

}
}
}
