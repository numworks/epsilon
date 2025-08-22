#pragma once

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Flash {

int TotalNumberOfSectors();
int SectorAtAddress(uint32_t address);
bool IncludesAddress(uint32_t address);

}  // namespace Flash
}  // namespace Device
}  // namespace Ion
