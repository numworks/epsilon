#pragma once

#include <assert.h>
#include <omg/unreachable.h>
#include <stdint.h>

#include "components.h"

namespace Escher {

constexpr int k_numberOfDataColors = 10;

/* Data colors are defined as fixed components in the ThemeComponent enum. A
 * data color component can be retrieved by its index */
constexpr ThemeComponent DataColorComponent(int index) {
  assert(index >= 0 && index < k_numberOfDataColors);
  return static_cast<ThemeComponent>(
      static_cast<uint8_t>(ThemeComponent::DataRed) + index);
}

constexpr ThemeComponent DataColorComponentLight(int index) {
  assert(index >= 0 && index < k_numberOfDataColors);
  return static_cast<ThemeComponent>(
      static_cast<uint8_t>(ThemeComponent::DataRedLight) + index);
}

}  // namespace Escher
