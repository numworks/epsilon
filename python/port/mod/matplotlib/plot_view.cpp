#include "plot_view.h"

namespace Matplotlib {

void PlotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);

  if (m_store->gridRequested()) {
    drawGrid(ctx, rect);
  }

  if (m_store->axesRequested()) {
    drawAxes(ctx, rect);
    drawLabelsAndGraduations(ctx, rect, Axis::Vertical, true);
    drawLabelsAndGraduations(ctx, rect, Axis::Horizontal, true);
  }

  for (PlotStore::Dot dot : m_store->dots()) {
    traceDot(ctx, rect, dot);
  }

  for (PlotStore::Label label : m_store->labels()) {
    traceLabel(ctx, rect, label);
  }

  for (PlotStore::Segment segment : m_store->segments()) {
    traceSegment(ctx, rect, segment);
  }

  for (PlotStore::Rect rectangle : m_store->rects()) {
    traceRect(ctx, rect, rectangle);
  }
}

void PlotView::traceDot(KDContext * ctx, KDRect r, PlotStore::Dot dot) const {
  drawDot(ctx, r, dot.x(), dot.y(), dot.color());
}

void PlotView::traceSegment(KDContext * ctx, KDRect r, PlotStore::Segment segment) const {
  drawSegment2(
    ctx, r,
    segment.xStart(), segment.yStart(),
    segment.xEnd(), segment.yEnd(),
    segment.color()
  );
}

void PlotView::traceRect(KDContext * ctx, KDRect r, PlotStore::Rect rect) const {
  KDRect pixelRect(
    floatToPixel(Axis::Horizontal, rect.x()),
    floatToPixel(Axis::Vertical, rect.y()),
    rect.width() / pixelWidth(),
    rect.height() / pixelHeight()
  );
  ctx->fillRect(pixelRect, rect.color());
}

void PlotView::traceLabel(KDContext * ctx, KDRect r, PlotStore::Label label) const {
  drawLabel(ctx, r,
    label.x(), label.y(), label.string(),
    KDColorBlack,
    RelativePosition::None,
    RelativePosition::None
  );
}


}
