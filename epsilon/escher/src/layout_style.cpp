#include <apps/global_preferences.h>
#include <escher/layout_style.h>

namespace Escher {

Poincare::LayoutStyle ToPoincareLayoutStyle(const LayoutStyle& layoutStyle) {
  return Poincare::LayoutStyle{
      ToKDGlyphStyle(layoutStyle.textStyle),
      SharedGlobalPrefs().color(layoutStyle.selectionColor),
      SharedGlobalPrefs().color(layoutStyle.requiredPlaceholderColor),
      SharedGlobalPrefs().color(layoutStyle.optionalPlaceholderColor),
      SharedGlobalPrefs().color(layoutStyle.piecewiseCommaColor)};
}

}  // namespace Escher
