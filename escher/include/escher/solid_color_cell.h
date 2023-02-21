#ifndef ESCHER_SOLID_COLOR_CELL_H
#define ESCHER_SOLID_COLOR_CELL_H

#include <escher/highlight_cell.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>

namespace Escher {

/* Simple HighlightCell with a solid color. */

class SolidColorCell : public HighlightCell {
 public:
  SolidColorCell(KDColor color) : HighlightCell(), m_color(color){};
  void drawRect(KDContext* ctx, KDRect rect) const override {
    ctx->fillRect(rect, m_color);
  }

 private:
  KDColor m_color;
};

}  // namespace Escher

#endif
