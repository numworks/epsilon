#pragma once

namespace Ion {
namespace Device {
namespace SerialNumber {

constexpr static int k_serialNumberLength = 16;

void copy(char* buffer);

}  // namespace SerialNumber
}  // namespace Device
}  // namespace Ion
