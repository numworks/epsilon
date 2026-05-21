#pragma once

#include <kandinsky/color.h>
#include <kandinsky/glyph.h>

namespace Poincare {

struct LayoutStyle {
  KDGlyph::Style textStyle;
  KDColor backgroundSelectionColor;
  KDColor requiredPlaceholderColor;
  KDColor optionalPlaceholderColor;
  KDColor piecewiseCommaColor;
};

}  // namespace Poincare
