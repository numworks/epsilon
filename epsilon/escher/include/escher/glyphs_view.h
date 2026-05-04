#pragma once

#include <escher/cell_widget.h>
#include <escher/menu_cell.h>
#include <escher/text_format.h>
#include <escher/theme/components.h>
#include <escher/theme/highlight.h>
#include <escher/view.h>

namespace Escher {

class GlyphsView : public View, public CellWidget {
 public:
  GlyphsView(const TextFormat& format = {}) { setGlyphFormat(format); }

  KDFont::Size font() const { return m_textFormat.style.font; }

  virtual KDGlyph::Format glyphFormat() const {
    return ToKDGlyphFormat(textFormat());
  }

  void setGlyphFormat(const TextFormat& format);
  void setFont(KDFont::Size font);
  void setTextColor(ThemeComponent textColor);
  void setBackgroundColor(ThemeComponent backgroundColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);

  // CellWidget
  const View* view() const override { return this; }
  void setHighlighted(bool highlighted) override {
    if (m_isHighlighted != highlighted) {
      m_isHighlighted = highlighted;
      markWholeFrameAsDirty();
    }
  }
  bool alwaysAlignWithLabelAsAccessory() const override { return true; }
  bool preventRightAlignedSubLabel(Type type) const override {
    return type == Type::Accessory;
  }

  constexpr static TextFormat k_defaultLabel = {};
  constexpr static TextFormat k_defaultSubLabel = {
      .style = {.glyphColor = ThemeComponent::TokenGrayDark,
                .font = KDFont::Size::Small}};
  constexpr static TextFormat k_defaultAccessory = {.horizontalAlignment =
                                                        KDGlyph::k_alignRight};

  static TextFormat FormatForWidgetType(CellWidget::Type type) {
    return type == CellWidget::Type::Label
               ? k_defaultLabel
               : (type == CellWidget::Type::SubLabel ? k_defaultSubLabel
                                                     : k_defaultAccessory);
  }

  void defaultInitialization(CellWidget::Type type) override {
    setGlyphFormat(FormatForWidgetType(type));
  }

 protected:
  // Takes highlight into account
  TextFormat textFormat() const {
    return TextFormat{
        .style =
            TextStyle{
                .glyphColor =
                    Highlight(m_textFormat.style.glyphColor, m_isHighlighted),
                .backgroundColor = Highlight(m_textFormat.style.backgroundColor,
                                             m_isHighlighted),
                .font = m_textFormat.style.font,
            },
        .horizontalAlignment = m_textFormat.horizontalAlignment,
        .verticalAlignment = m_textFormat.verticalAlignment};
  }

 private:
  TextFormat m_textFormat;
  bool m_isHighlighted = false;
};

}  // namespace Escher
