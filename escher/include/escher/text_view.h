#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/cell_widget.h>
#include <escher/palette.h>
#include <escher/view.h>
#include <kandinsky/color.h>

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

  typedef struct {
    KDFont::Size fontSize;
    KDColor textColor;
    float horizontalAlginment;
    float verticalAlignment;
  } InitializationParameters;
  constexpr static InitializationParameters k_labelInitializationParameters = {
      KDFont::Size::Large, KDColorBlack, KDContext::k_alignLeft,
      KDContext::k_alignCenter};
  constexpr static InitializationParameters k_subLabelInitializationParameters =
      {KDFont::Size::Small, Palette::GrayDark, KDContext::k_alignLeft,
       KDContext::k_alignCenter};
  constexpr static InitializationParameters
      k_accessoryInitializationParameters = {KDFont::Size::Large, KDColorBlack,
                                             KDContext::k_alignRight,
                                             KDContext::k_alignCenter};
  void defaultInitialization(CellWidget::Type type) override;

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
