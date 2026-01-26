#pragma once

#include <stdint.h>

namespace Ion {
namespace Reset {

void core();
/* Delete external apps before resetting.
 * This is used specifically when resetting the device from the settings,
 * because only hardware resets remove external apps at userland startup. */
void coreDeleteApps();

enum class ResetType : bool {
  Hardware,  // POWER on, NRST pin
  Software   // crash or Reset::core()
};

ResetType lastResetType();

}  // namespace Reset
}  // namespace Ion
