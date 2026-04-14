#pragma once

#include <poincare/layout_style.h>

#include "text_format.h"
#include "theme/components.h"

namespace Escher {

// The layout colors are theme-dependent
struct LayoutStyle {
  TextStyle textStyle = {};
  ThemeComponent selectionColor = ThemeComponent::Select;
  ThemeComponent requiredPlaceholderColor = ThemeComponent::TokenYellow;
  ThemeComponent optionalPlaceholderColor = ThemeComponent::TokenGrayBright;
  ThemeComponent piecewiseCommaColor = ThemeComponent::TokenGrayDark;
};

Poincare::LayoutStyle ToPoincareLayoutStyle(const LayoutStyle& layoutStyle);

constexpr static LayoutStyle DefaultLayoutStyle(const TextStyle& textStyle) {
  return {.textStyle = textStyle};
}

}  // namespace Escher
