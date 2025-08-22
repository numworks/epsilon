#pragma once

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace Timing {
namespace Config {

constexpr static int LoopsPerMillisecondLowFrequency = 4811;
constexpr static int LoopsPerMicrosecond = 38;

}  // namespace Config
}  // namespace Timing
}  // namespace Device
}  // namespace Ion
