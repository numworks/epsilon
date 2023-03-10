#ifndef ESCHER_TEXT_VIEW_H
#define ESCHER_TEXT_VIEW_H

#include <escher/cell_widget.h>
#include <escher/glyphs_view.h>
#include <escher/palette.h>

namespace Escher {

class TextView : public GlyphsView, public CellWidget {
 public:
  TextView(CellWidget::Type type = CellWidget::Type::Label) {
    defaultInitialization(type);
  }

  TextView(KDFont::Size font, float horizontalAlignment,
           float verticalAlignment, KDColor textColor = KDColorBlack,
           KDColor backgroundColor = KDColorWhite)
      : GlyphsView(font, horizontalAlignment, verticalAlignment, textColor,
                   backgroundColor) {}

  // View
  void drawRect(KDContext* ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;

  virtual const char* text() const = 0;
  virtual void setText(const char* text) = 0;

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

#if ESCHER_VIEW_LOGGING
 protected:
  const char* className() const override { return "TextView"; }
#endif
};

}  // namespace Escher
#endif
