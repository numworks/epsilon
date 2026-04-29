#pragma once

#include "components.h"

namespace Escher {

/* Returns the highlighted version of a ThemeComponent if isHighlighted is true.
 * Returns the component otherwise. Useful for text and background highlights */
constexpr static ThemeComponent Highlight(ThemeComponent component,
                                          bool isHighlighted) {
  if (!isHighlighted) {
    return component;
  }
  switch (component) {
    case ThemeComponent::TextDefault:
      return ThemeComponent::TextSelect;
    case ThemeComponent::TextGrayDark:
      return ThemeComponent::TextSelectGrayDark;
    case ThemeComponent::TextGrayVeryDark:
      return ThemeComponent::TextSelectGrayVeryDark;
    case ThemeComponent::TextPurple:
      return ThemeComponent::TextSelectPurple;
    case ThemeComponent::BackgroundWhite:
    case ThemeComponent::WallScreen:
      return ThemeComponent::SurfaceSelect;
    default:
      return component;
  }
}

}  // namespace Escher
