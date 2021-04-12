#include <drivers/external_flash.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

void init() {}
void shutdown() {}

void MassErase() {}
int SectorAtAddress(uint32_t) { return 0; }
void EraseSector(int) {}
void WriteMemory(uint8_t *, const uint8_t *, size_t) {}
void JDECid(uint8_t *, uint8_t *, uint8_t *) {}

}
}
}
