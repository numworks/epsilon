#include <escher/layout_style.h>
#include <escher/theme/color_picker.h>

namespace Escher {

Poincare::LayoutStyle ToPoincareLayoutStyle(const LayoutStyle& layoutStyle) {
  return Poincare::LayoutStyle{
      ToKDGlyphStyle(layoutStyle.textStyle),
      ColorPicker::Resolve(layoutStyle.selectionColor),
      ColorPicker::Resolve(layoutStyle.requiredPlaceholderColor),
      ColorPicker::Resolve(layoutStyle.optionalPlaceholderColor),
      ColorPicker::Resolve(layoutStyle.piecewiseCommaColor)};
}

}  // namespace Escher
