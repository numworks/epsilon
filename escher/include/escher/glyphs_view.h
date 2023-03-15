#ifndef ESCHER_GLYPHS_VIEW_H
#define ESCHER_GLYPHS_VIEW_H

#include <escher/cell_widget.h>
#include <escher/palette.h>
#include <escher/view.h>

namespace Escher {

/* This is outside of the class because Clang has a bug that prevent default
 * args to be used if the struct is within the class. */

class GlyphsView : public View, public CellWidget {
 public:
  GlyphsView(KDGlyph::Format format = {}) { setGlyphFormat(format); }

  KDFont::Size font() const { return m_glyphFormat.style.font; }

  void setGlyphFormat(KDGlyph::Format format);
  void setFont(KDFont::Size font);
  void setTextColor(KDColor textColor);
  void setAlignment(float horizontalAlignment, float verticalAlignment);

  // CellWidget
  const View* view() const override { return this; }
  void setBackgroundColor(KDColor backgroundColor) override;
  bool alwaysAlignWithLabelAsAccessory() const override { return true; }
  bool preventRightAlignedSubLabel(Type type) const override {
    return type == Type::Accessory;
  }

  constexpr static KDGlyph::Format k_defaultLabel = {};
  constexpr static KDGlyph::Format k_defaultSubLabel = {
      {.glyphColor = Palette::GrayDark, .font = KDFont::Size::Small}};
  constexpr static KDGlyph::Format k_defaultAccessory = {
      .horizontalAlignment = KDGlyph::k_alignRight};

  static KDGlyph::Format FormatForWidgetType(CellWidget::Type type) {
    return type == CellWidget::Type::Label
               ? k_defaultLabel
               : (type == CellWidget::Type::SubLabel ? k_defaultSubLabel
                                                     : k_defaultAccessory);
  }

  void defaultInitialization(CellWidget::Type type) override {
    setGlyphFormat(FormatForWidgetType(type));
  }

 protected:
  KDGlyph::Format m_glyphFormat;
};

}  // namespace Escher
#endif
