#include <drivers/external_flash.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

// Dummy implementation: N0100 has no external flash

void init() {
}

void shutdown() {
}

int NumberOfSectors() {
  return 0;
}

int SectorAtAddress(uint32_t address) {
  return 0;
}

void MassErase() {}

void EraseSector(int i) {}

void WriteMemory(uint8_t * destination, uint8_t * source, size_t length) {}

}
}
}
