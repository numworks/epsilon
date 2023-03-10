#ifndef ESCHER_GLYPHS_VIEW_H
#define ESCHER_GLYPHS_VIEW_H

#include <escher/view.h>

namespace Escher {

class GlyphsView : public View {
 public:
  GlyphsView(KDFont::Size font = KDFont::Size::Large,
             float horizontalAlignment = KDContext::k_alignLeft,
             float verticalAlignment = KDContext::k_alignCenter,
             KDColor textColor = KDColorBlack,
             KDColor backgroundColor = KDColorWhite)
      : m_font(font),
        m_textColor(textColor),
        m_backgroundColor(backgroundColor),
        m_horizontalAlignment(horizontalAlignment),
        m_verticalAlignment(verticalAlignment) {}

  KDFont::Size font() const { return m_font; }

  void setFont(KDFont::Size font) {
    m_font = font;
    markRectAsDirty(bounds());
  }

  void setTextColor(KDColor textColor) {
    if (textColor != m_textColor) {
      m_textColor = textColor;
      markRectAsDirty(bounds());
    }
  }

  void setBackgroundColor(KDColor backgroundColor) {
    if (m_backgroundColor != backgroundColor) {
      m_backgroundColor = backgroundColor;
      markRectAsDirty(bounds());
    }
  }

  void setAlignment(float horizontalAlignment, float verticalAlignment) {
    m_horizontalAlignment = horizontalAlignment;
    m_verticalAlignment = verticalAlignment;
    markRectAsDirty(bounds());
  }

 protected:
  KDFont::Size m_font;
  KDColor m_textColor;
  KDColor m_backgroundColor;
  float m_horizontalAlignment;
  float m_verticalAlignment;
};

}  // namespace Escher
#endif
