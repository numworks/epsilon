#pragma once

#include <kandinsky/glyph.h>

#include "theme/components.h"

namespace Escher {

// The text style colors are theme-dependent
struct TextStyle {
  ThemeComponent glyphColor = ThemeComponent::TextDefault;
  ThemeComponent backgroundColor = ThemeComponent::BackgroundWhite;
  KDFont::Size font = KDFont::Size::Large;
};

KDGlyph::Style ToKDGlyphStyle(const TextStyle& textStyle);

struct TextFormat {
  TextStyle style;
  float horizontalAlignment = KDGlyph::k_alignLeft;
  float verticalAlignment = KDGlyph::k_alignCenter;
};

KDGlyph::Format ToKDGlyphFormat(const TextFormat& textFormat);

}  // namespace Escher
