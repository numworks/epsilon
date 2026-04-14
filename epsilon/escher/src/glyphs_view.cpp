#include <escher/glyphs_view.h>

namespace Escher {
void GlyphsView::setGlyphFormat(const TextFormat& format) {
  m_glyphFormat = format;
  markWholeFrameAsDirty();
}

void GlyphsView::setFont(KDFont::Size font) {
  m_glyphFormat.style.font = font;
  markWholeFrameAsDirty();
}

void GlyphsView::setTextColor(ThemeComponent textColor) {
  if (textColor != m_glyphFormat.style.glyphColor) {
    m_glyphFormat.style.glyphColor = textColor;
    markWholeFrameAsDirty();
  }
}

void GlyphsView::setBackgroundColor(ThemeComponent backgroundColor) {
  if (m_glyphFormat.style.backgroundColor != backgroundColor) {
    m_glyphFormat.style.backgroundColor = backgroundColor;
    markWholeFrameAsDirty();
  }
}

void GlyphsView::setAlignment(float horizontalAlignment,
                              float verticalAlignment) {
  m_glyphFormat.horizontalAlignment = horizontalAlignment;
  m_glyphFormat.verticalAlignment = verticalAlignment;
  markWholeFrameAsDirty();
}

}  // namespace Escher
