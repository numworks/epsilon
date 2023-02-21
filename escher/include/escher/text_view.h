#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/view.h>
#include <kandinsky/color.h>

/* alignment = 0 -> align left or top
 * alignment = 0.5 -> align center
 * alignment = 1.0 -> align right or bottom */

namespace Escher {

class TextView : public View {
 public:
  TextView(KDFont::Size font = KDFont::Size::Large,
           float horizontalAlignment = KDContext::k_alignLeft,
           float verticalAlignment = KDContext::k_alignTop,
           KDColor textColor = KDColorBlack,
           KDColor backgroundColor = KDColorWhite)
      : View(),
        m_font(font),
        m_horizontalAlignment(horizontalAlignment),
        m_verticalAlignment(verticalAlignment),
        m_textColor(textColor),
        m_backgroundColor(backgroundColor) {}
  void drawRect(KDContext* ctx, KDRect rect) const override;
  void setBackgroundColor(KDColor backgroundColor);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  KDSize minimalSizeForOptimalDisplay() const override;
  virtual const char* text() const = 0;
  virtual void setText(const char* text) = 0;
  KDFont::Size font() const { return m_font; }
  void setFont(KDFont::Size font);

 protected:
#if ESCHER_VIEW_LOGGING
  const char* className() const override { return "TextView"; }
#endif
  KDFont::Size m_font;
  float m_horizontalAlignment;
  float m_verticalAlignment;
  KDColor m_textColor;
  KDColor m_backgroundColor;
};

}  // namespace Escher
#endif
