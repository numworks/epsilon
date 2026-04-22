#pragma once

#include <assert.h>
#include <escher/palette.h>
#include <omg/unreachable.h>
#include <stdint.h>

#include "components.h"

namespace Escher {

constexpr int k_numberOfDataColors = 10;
static_assert(k_numberOfDataColors == Escher::Palette::k_numberOfDataColors);

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

/* Helper to convert a raw color to a ThemeComponent. "color" must be one of
 * the DataColors.
 * TODO: remove this helper when raw data colors are no more used */
constexpr ThemeComponent DataColorToComponent(KDColor color) {
  for (int i = 0; i < k_numberOfDataColors; i++) {
    if (Palette::DataColor[i] == color) {
      return DataColorComponent(i);
    }
  }
  OMG_UNREACHABLE
}

}  // namespace Escher
