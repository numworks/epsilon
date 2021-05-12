#ifndef SOLID_COLOR_CELL_H
#define SOLID_COLOR_CELL_H

#include <escher/highlight_cell.h>
#include <kandinsky/color.h>
#include <kandinsky/context.h>
#include <kandinsky/rect.h>

using namespace Escher;

namespace Probability {

// TODO anything better?
/* Simple HighlightCell with a solid color. */
class SolidColorCell : public HighlightCell {
 public:
  SolidColorCell(KDColor color) : HighlightCell(), m_color(color){};
  void drawRect(KDContext * ctx, KDRect rect) const override { ctx->fillRect(rect, m_color); }

 private:
  KDColor m_color;
};

}  // namespace Probability

#endif /* SOLID_COLOR_CELL_H */
