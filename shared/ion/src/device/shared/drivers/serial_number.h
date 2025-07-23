#pragma once

namespace Ion {
namespace Device {
namespace SerialNumber {

constexpr static int k_serialNumberLength =
#if ION_VARIANT_EPSILON
    16;
#elif ION_VARIANT_SCANDIUM
    18;
#endif

void copy(char* buffer);

}  // namespace SerialNumber
}  // namespace Device
}  // namespace Ion
