#pragma once

#include <poincare/layout_style.h>

#include "text_format.h"
#include "theme/components.h"

namespace Escher {

// The layout colors are theme-dependent
struct LayoutStyle {
  TextStyle textStyle = {};
  ThemeComponent backgroundSelectionColor = ThemeComponent::SurfaceSelect;
  ThemeComponent glyphSelectionColor = ThemeComponent::TextSelect;
  ThemeComponent requiredPlaceholderColor =
      ThemeComponent::ExpressionRequiredPlaceholder;
  ThemeComponent optionalPlaceholderColor =
      ThemeComponent::ExpressionOptionalPlaceholder;
  ThemeComponent piecewiseCommaColor = ThemeComponent::TextGrayDark;
};

Poincare::LayoutStyle ToPoincareLayoutStyle(const LayoutStyle& layoutStyle);

constexpr static LayoutStyle DefaultLayoutStyle(const TextStyle& textStyle) {
  return {.textStyle = textStyle};
}

}  // namespace Escher
