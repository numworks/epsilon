#pragma once

#include "components.h"

namespace Escher {

/* Returns the highlighted version of a ThemeComponent. Mostly useful for text
 * highlights */
constexpr static ThemeComponent Highlight(ThemeComponent component) {
  switch (component) {
    case ThemeComponent::TextDefault:
      return ThemeComponent::TextSelect;
    case ThemeComponent::TextGrayDark:
      return ThemeComponent::TextSelectGrayDark;
    case ThemeComponent::TextGrayVeryDark:
      return ThemeComponent::TextSelectGrayVeryDark;
    default:
      return component;
  }
}

}  // namespace Escher
