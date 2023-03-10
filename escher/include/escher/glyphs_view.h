#ifndef ESCHER_GLYPHS_VIEW_H
#define ESCHER_GLYPHS_VIEW_H

#include <escher/cell_widget.h>
#include <escher/palette.h>
#include <escher/view.h>

namespace Escher {

/* This is outside of the class because Clang has a bug that prevent default
 * args to be used if the struct is within the class. */
struct GlyphsParameters {
  KDFont::Size fontSize = KDFont::Size::Large;
  float horizontalAlginment = KDContext::k_alignLeft;
  float verticalAlignment = KDContext::k_alignCenter;
  KDColor textColor = KDColorBlack;
  KDColor backgroundColor = KDColorWhite;
};

class GlyphsView : public View, public CellWidget {
 public:
  GlyphsView(CellWidget::Type type = CellWidget::Type::Label) {
    defaultInitialization(type);
  }

  GlyphsView(KDFont::Size font,
             float horizontalAlignment = k_defaultLabel.horizontalAlginment,
             float verticalAlignment = k_defaultLabel.verticalAlignment,
             KDColor textColor = k_defaultLabel.textColor,
             KDColor backgroundColor = k_defaultLabel.backgroundColor)
      : m_font(font),
        m_textColor(textColor),
        m_backgroundColor(backgroundColor),
        m_horizontalAlignment(horizontalAlignment),
        m_verticalAlignment(verticalAlignment) {}

  KDFont::Size font() const { return m_font; }
  void setFont(KDFont::Size font);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);

  // CellWidget
  void setBackgroundColor(KDColor backgroundColor) override;
  const View* view() const override { return this; }

  constexpr static GlyphsParameters k_defaultLabel = {};
  constexpr static GlyphsParameters k_defaultSubLabel = {
      .fontSize = KDFont::Size::Small, .textColor = Palette::GrayDark};
  constexpr static GlyphsParameters k_defaultAccessory = {
      .horizontalAlginment = KDContext::k_alignRight};

  void defaultInitialization(CellWidget::Type type) override;

 protected:
  KDFont::Size m_font;
  KDColor m_textColor;
  KDColor m_backgroundColor;
  float m_horizontalAlignment;
  float m_verticalAlignment;
};

}  // namespace Escher
#endif
