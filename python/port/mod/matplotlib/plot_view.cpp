#include "plot_view.h"

namespace Matplotlib {

void PlotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  if (m_store->grid()) {
    drawGrid(ctx, rect);
  }
  drawAxes(ctx, rect);
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, true);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, true);

  for (PlotStore::Dot dot : m_store->dots()) {
    drawDot(ctx, rect, dot.x(), dot.y(), dot.color());
  }
}

}
