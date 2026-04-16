#include <escher/text_format.h>
#include <escher/theme/color_picker.h>

namespace Escher {

KDGlyph::Style ToKDGlyphStyle(const TextStyle& textStyle) {
  return KDGlyph::Style{
      .glyphColor = ColorPicker::Resolve(textStyle.glyphColor),
      .backgroundColor = ColorPicker::Resolve(textStyle.backgroundColor),
      .font = textStyle.font};
}

KDGlyph::Format ToKDGlyphFormat(const TextFormat& textFormat) {
  return KDGlyph::Format{.style = ToKDGlyphStyle(textFormat.style),
                         .horizontalAlignment = textFormat.horizontalAlignment,
                         .verticalAlignment = textFormat.verticalAlignment};
}

}  // namespace Escher
