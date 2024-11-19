#ifndef ESCHER_SOLID_COLOR_CELL_H
#define ESCHER_SOLID_COLOR_CELL_H

#include <escher/highlight_cell.h>
#include <kandinsky/color.h>

namespace Escher {

/* Simple empty HighlightCell with a solid color. */

class SolidColorCell : public HighlightCell {
 public:
  SolidColorCell(KDColor color = KDColorWhite)
      : HighlightCell(), m_color(color) {}
  void drawRect(KDContext* ctx, KDRect rect) const override {
    ctx->fillRect(rect, isHighlighted() ? m_highlightColor : m_color);
  }
  void setColor(KDColor color) { m_color = color; }
  void setHighlightColor(KDColor color) { m_highlightColor = color; }

 private:
  KDColor m_color;
  KDColor m_highlightColor = KDColorWhite;
};

}  // namespace Escher

#endif
