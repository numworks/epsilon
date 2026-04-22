#pragma once

#include <kandinsky/glyph.h>

#include <type_traits>

#include "glyphs_view.h"
#include "text_format.h"

namespace Escher {

struct TextColors {
  KDColor glyphColor = KDBlack;
  KDColor backgroundColor = KDWhite;
};

struct TextFormatWithoutColors {
  KDFont::Size font = KDFont::Size::Large;
  float horizontalAlignment = KDGlyph::k_alignLeft;
  float verticalAlignment = KDGlyph::k_alignCenter;
};

/*  Conversion from TextFormatWithoutColors to TextFormat (colors are left at
 * their default ThemeComponent values and will be ignored) */
inline TextFormat MakeTextFormat(const TextFormatWithoutColors& format) {
  return TextFormat{.style = TextStyle{.font = format.font},
                    .horizontalAlignment = format.horizontalAlignment,
                    .verticalAlignment = format.verticalAlignment};
}

/* RawColor is a special version of any GlyphsView-derived class that works
 * with raw glyph and background colors instead of ThemeComponent.
 * It is used by the RawColorMessageTextView class (used in Elements) and by the
 * RawColorBufferTextView (that is temporary until SolidColorView uses
 * ThemeComponents). When RawColorBufferTextView is not useful anymore and if we
 * include the elements colors to the ThemeComponents enum, then we can remove
 * RawColor. */
template <typename Base>
class RawColor : public Base {
 public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<GlyphsView, Base>(),
                "RawColor<Base> requires Base to derive from GlyphsView");
#endif

  using Base::Base;  // Inherit all constructors from Base

  KDGlyph::Format glyphFormat() const override {
    return KDGlyph::Format{
        .style = KDGlyph::Style{.glyphColor = m_rawColors.glyphColor,
                                .backgroundColor = m_rawColors.backgroundColor,
                                .font = this->m_textFormat.style.font},
        .horizontalAlignment = this->m_textFormat.horizontalAlignment,
        .verticalAlignment = this->m_textFormat.verticalAlignment};
  }

  void setTextColor(KDColor color) {
    if (m_rawColors.glyphColor != color) {
      m_rawColors.glyphColor = color;
      this->markWholeFrameAsDirty();
    }
  }
  void setBackgroundColor(KDColor color) {
    if (m_rawColors.backgroundColor != color) {
      m_rawColors.backgroundColor = color;
      this->markWholeFrameAsDirty();
    }
  }

  // Disable ThemeComponent-based setters
  void setGlyphFormat(const TextFormat&) = delete;
  void setTextColor(ThemeComponent) = delete;
  void setBackgroundColor(ThemeComponent) = delete;

 protected:
  TextColors m_rawColors;
};

}  // namespace Escher
