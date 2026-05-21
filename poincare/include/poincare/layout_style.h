#pragma once

#include <kandinsky/color.h>
#include <kandinsky/glyph.h>

namespace Poincare {

struct LayoutStyle {
  KDGlyph::Style textStyle;
  KDColor backgroundSelectionColor;
#if !POINCARE_SCANDIUM_LAYOUTS
  // Scandium layouts use the background color as the glyph selection color
  KDColor glyphSelectionColor;
#endif
  KDColor requiredPlaceholderColor;
  KDColor optionalPlaceholderColor;
  KDColor piecewiseCommaColor;
};

}  // namespace Poincare
