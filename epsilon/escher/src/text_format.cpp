#include <apps/global_preferences.h>
#include <escher/text_format.h>

namespace Escher {

KDGlyph::Style ToKDGlyphStyle(const TextStyle& textStyle) {
  return KDGlyph::Style{
      .glyphColor = SharedGlobalPrefs().color(textStyle.glyphColor),
      .backgroundColor = SharedGlobalPrefs().color(textStyle.backgroundColor),
      .font = textStyle.font};
}

KDGlyph::Format ToKDGlyphFormat(const TextFormat& textFormat) {
  return KDGlyph::Format{.style = ToKDGlyphStyle(textFormat.style),
                         .horizontalAlignment = textFormat.horizontalAlignment,
                         .verticalAlignment = textFormat.verticalAlignment};
}

}  // namespace Escher
