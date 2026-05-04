#pragma once

#include <omg/unreachable.h>

#include "components.h"
#include "data_colors.h"

namespace Escher {

/* Returns the highlighted version of a ThemeComponent if isHighlighted is true.
 * Returns the component otherwise. Useful for text and background highlights */
constexpr static ThemeComponent Highlight(ThemeComponent component,
                                          bool isHighlighted) {
  if (!isHighlighted) {
    return component;
  }
  if (IsDataColor(component) || IsDataColorLight(component)) {
    // Data colors do not change when highlighted
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
    case ThemeComponent::TextWhite:
    case ThemeComponent::TokenYellow:
    case ThemeComponent::SurfaceSelect:
      // These colors do not change when highlighted
      return component;
    default:
      /* Other components should not be highlightable */
      OMG_UNREACHABLE
  }
}

}  // namespace Escher
