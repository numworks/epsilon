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

  for (PlotStore::Label label : m_store->labels()) {
    drawLabel(ctx, rect,
      label.x(), label.y(), label.string(),
      KDColorBlack,
      CurveView::RelativePosition::Before,
      CurveView::RelativePosition::None
    );
  }

  for (PlotStore::Segment segment : m_store->segments()) {
    drawSegment2(
      ctx, rect,
      segment.xStart(), segment.yStart(),
      segment.xEnd(), segment.yEnd(),
      segment.color()
    );
  }

}

}
