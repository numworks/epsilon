#include <escher/layout_style.h>
#include <escher/theme/color_picker.h>

namespace Escher {

Poincare::LayoutStyle ToPoincareLayoutStyle(const LayoutStyle& layoutStyle) {
  return Poincare::LayoutStyle{
      .textStyle = ToKDGlyphStyle(layoutStyle.textStyle),
      .backgroundSelectionColor =
          ColorPicker::Resolve(layoutStyle.backgroundSelectionColor),
      .glyphSelectionColor =
          ColorPicker::Resolve(layoutStyle.glyphSelectionColor),
      .requiredPlaceholderColor =
          ColorPicker::Resolve(layoutStyle.requiredPlaceholderColor),
      .optionalPlaceholderColor =
          ColorPicker::Resolve(layoutStyle.optionalPlaceholderColor),
      .piecewiseCommaColor =
          ColorPicker::Resolve(layoutStyle.piecewiseCommaColor)};
}

}  // namespace Escher
