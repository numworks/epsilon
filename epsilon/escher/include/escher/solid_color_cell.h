#pragma once

#include <escher/highlight_cell.h>
#include <escher/theme/color_picker.h>
#include <kandinsky/color.h>

namespace Escher {

/* Simple empty HighlightCell with a solid color. */

class SolidColorCell : public HighlightCell {
 public:
  SolidColorCell(ThemeComponent color = ThemeComponent::BackgroundWhite)
      : HighlightCell(), m_color(color) {}
  void drawRect(KDContext* ctx, KDRect rect) const override {
    ctx->fillRect(rect, ColorPicker::Resolve(m_color));
  }
  void setColor(ThemeComponent color) { m_color = color; }

 private:
  ThemeComponent m_color;
};

}  // namespace Escher
