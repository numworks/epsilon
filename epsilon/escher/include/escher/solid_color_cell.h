#pragma once

#include <escher/highlight_cell.h>
#include <escher/theme/color_picker.h>
#include <kandinsky/color.h>

namespace Escher {

/* Simple empty HighlightCell with a solid color. */

class SolidColorCell : public HighlightCell {
 public:
  SolidColorCell(
      KDColor color = ColorPicker::Resolve(ThemeComponent::BackgroundWhite))
      : HighlightCell(), m_color(color) {}
  void drawRect(KDContext* ctx, KDRect rect) const override {
    ctx->fillRect(rect, m_color);
  }
  void setColor(KDColor color) { m_color = color; }

 private:
  KDColor m_color;
};

}  // namespace Escher
