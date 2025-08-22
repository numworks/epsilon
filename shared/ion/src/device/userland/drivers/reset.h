#pragma once

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Reset {

void jump(uint32_t jumpIsrVectorAddress);

}
}  // namespace Device
}  // namespace Ion
