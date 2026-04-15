#pragma once

#include <kandinsky/glyph.h>

#include "escher/message_text_view.h"

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

/* RawColorMessageTextView is a special version of GlyphsView which works with
 * raw glyph and background colors */
class RawColorMessageTextView : public MessageTextView {
 public:
  RawColorMessageTextView(I18n::Message message = (I18n::Message)0,
                          const TextFormatWithoutColors& format = {},
                          const TextColors& colors = {},
                          KDCoordinate lineSpacing = 0);

  KDGlyph::Format glyphFormat() const override {
    return KDGlyph::Format{
        .style = KDGlyph::Style{.glyphColor = m_rawColors.glyphColor,
                                .backgroundColor = m_rawColors.backgroundColor,
                                .font = m_textFormat.style.font},
        .horizontalAlignment = m_textFormat.horizontalAlignment,
        .verticalAlignment = m_textFormat.verticalAlignment};
  }

  void setTextColor(KDColor color);
  void setBackgroundColor(KDColor color);

  // Disable ThemeComponents-based setters
  void setGlyphFormat(const TextFormat&) = delete;
  void setTextColor(ThemeComponent) = delete;
  void setBackgroundColor(ThemeComponent) = delete;

 private:
  TextColors m_rawColors;
};

}  // namespace Escher
