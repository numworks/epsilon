#include "plot_view.h"

namespace Matplotlib {

void PlotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  drawLabelsAndGraduations(ctx, rect, Axis::Vertical, true);
  drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, true);
}

}
