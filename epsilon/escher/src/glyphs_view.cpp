#include <escher/glyphs_view.h>

namespace Escher {
void GlyphsView::setGlyphFormat(const TextFormat& format) {
  m_textFormat = format;
  markWholeFrameAsDirty();
}

void GlyphsView::setFont(KDFont::Size font) {
  m_textFormat.style.font = font;
  markWholeFrameAsDirty();
}

void GlyphsView::setTextColor(ThemeComponent textColor) {
  if (textColor != m_textFormat.style.glyphColor) {
    m_textFormat.style.glyphColor = textColor;
    markWholeFrameAsDirty();
  }
}

void GlyphsView::setBackgroundColor(ThemeComponent backgroundColor) {
  if (m_textFormat.style.backgroundColor != backgroundColor) {
    m_textFormat.style.backgroundColor = backgroundColor;
    markWholeFrameAsDirty();
  }
}

void GlyphsView::setAlignment(float horizontalAlignment,
                              float verticalAlignment) {
  m_textFormat.horizontalAlignment = horizontalAlignment;
  m_textFormat.verticalAlignment = verticalAlignment;
  markWholeFrameAsDirty();
}

}  // namespace Escher
