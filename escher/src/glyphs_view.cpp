#include <escher/glyphs_view.h>

namespace Escher {
void GlyphsView::setGlyphFormat(KDGlyph::Format format) {
  m_glyphFormat = format;
  markRectAsDirty(bounds());
}

void GlyphsView::setFont(KDFont::Size font) {
  m_glyphFormat.style.font = font;
  markRectAsDirty(bounds());
}

void GlyphsView::setTextColor(KDColor textColor) {
  if (textColor != m_glyphFormat.style.glyphColor) {
    m_glyphFormat.style.glyphColor = textColor;
    markRectAsDirty(bounds());
  }
}

void GlyphsView::setBackgroundColor(KDColor backgroundColor) {
  if (m_glyphFormat.style.backgroundColor != backgroundColor) {
    m_glyphFormat.style.backgroundColor = backgroundColor;
    markRectAsDirty(bounds());
  }
}

void GlyphsView::setAlignment(float horizontalAlignment,
                              float verticalAlignment) {
  m_glyphFormat.horizontalAlignment = horizontalAlignment;
  m_glyphFormat.verticalAlignment = verticalAlignment;
  markRectAsDirty(bounds());
}

}  // namespace Escher
