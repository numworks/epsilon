#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace InternalFlash {
namespace Config {

constexpr int NumberOfSectors = 4;
constexpr uint32_t SectorAddresses[NumberOfSectors + 1] = {
    0x08000000, 0x08020000, 0x08040000, 0x08060000, 0x08080000,
};

}  // namespace Config
}  // namespace InternalFlash
}  // namespace Device
}  // namespace Ion
