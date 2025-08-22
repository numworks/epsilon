#pragma once

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

void updateClearanceLevelForUnauthenticatedUserland(uint32_t leaveAddress);
void updateClearanceLevelForExternalApps();

}  // namespace Board
}  // namespace Device
}  // namespace Ion
