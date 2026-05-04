#pragma once

#include <assert.h>
#include <omg/unreachable.h>
#include <stdint.h>

#include "components.h"

namespace Escher {

constexpr int k_numberOfDataColors = 10;

constexpr bool IsDataColor(ThemeComponent component) {
  return (static_cast<uint8_t>(component) >=
          static_cast<uint8_t>(ThemeComponent::DataRed)) &&
         (static_cast<uint8_t>(component) <
          static_cast<uint8_t>(ThemeComponent::DataRed) + k_numberOfDataColors);
}

constexpr bool IsDataColorLight(ThemeComponent component) {
  return (static_cast<uint8_t>(component) >=
          static_cast<uint8_t>(ThemeComponent::DataRedLight)) &&
         (static_cast<uint8_t>(component) <
          static_cast<uint8_t>(ThemeComponent::DataRedLight) +
              k_numberOfDataColors);
}

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
