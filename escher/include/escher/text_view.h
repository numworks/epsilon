#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/cell_widget.h>
#include <escher/palette.h>
#include <escher/view.h>
#include <kandinsky/color.h>

/* alignment = 0 -> align left or top
 * alignment = 0.5 -> align center
 * alignment = 1.0 -> align right or bottom */

namespace Escher {

class TextView : public View, public CellWidget {
 public:
  TextView(CellWidget::Type type = CellWidget::Type::Label) {
    defaultInitialization(type);
  }

  TextView(KDFont::Size font, float horizontalAlignment,
           float verticalAlignment, KDColor textColor = KDColorBlack,
           KDColor backgroundColor = KDColorWhite)
      : View(),
        m_font(font),
        m_horizontalAlignment(horizontalAlignment),
        m_verticalAlignment(verticalAlignment),
        m_textColor(textColor),
        m_backgroundColor(backgroundColor) {}

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  // Getters
  virtual const char* text() const = 0;
  KDFont::Size font() const { return m_font; }

  // Setters
  virtual void setText(const char* text) = 0;
  void setTextColor(KDColor textColor);
  void setBackgroundColor(KDColor backgroundColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);
  void setFont(KDFont::Size font);

  // CellWidget
  const View* view() const override { return this; }
  void defaultInitialization(CellWidget::Type type) override {
    bool isLabel = type == CellWidget::Type::Label;
    setFont(isLabel ? KDFont::Size::Large : KDFont::Size::Small);
    setTextColor(isLabel ? KDColorBlack : Palette::GrayDark);
    setBackgroundColor(KDColorWhite);
    setAlignment(KDContext::k_alignLeft,
                 isLabel ? KDContext::k_alignTop : KDContext::k_alignCenter);
  }
  void setWidgetBackgroundColor(KDColor color) override {
    setBackgroundColor(color);
  }

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
