#pragma once

#include <stdint.h>

namespace Ion {
namespace Reset {

void core();

enum class ResetType : uint8_t {
  Hardware,  // POWER on, NRST pin
  Software   // crash or Reset::core()
};

ResetType lastResetType();

}  // namespace Reset
}  // namespace Ion
