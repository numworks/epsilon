#pragma once

#include <stdint.h>

namespace Ion {
namespace Reset {

void core();

enum class ResetType : bool {
  Hardware,  // POWER on, NRST pin
  Software   // crash or Reset::core()
};

ResetType lastResetType();

}  // namespace Reset
}  // namespace Ion
