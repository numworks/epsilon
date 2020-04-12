#include "plot_view.h"
#include <algorithm>

namespace Matplotlib {

void PlotView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);

  if (m_store->gridRequested()) {
    drawGrid(ctx, rect);
  }

  // Draw labels below all figures because they're drawn on a white rectangle.
  // TODO: we could blend them in the background by adding a parameter to drawLabelsAndGraduations.
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
  drawSegment(
    ctx, r,
    segment.xStart(), segment.yStart(),
    segment.xEnd(), segment.yEnd(),
    segment.color()
  );
  if (segment.isArrow()) {
    float dx = segment.xEnd() - segment.xStart();
    float dy = segment.yEnd() - segment.yStart();
    drawArrow(ctx, r, segment.xEnd(), segment.yEnd(), dx, dy, segment.color());
  }
}

void PlotView::traceRect(KDContext * ctx, KDRect r, PlotStore::Rect rect) const {
  KDCoordinate left = std::round(floatToPixel(Axis::Horizontal, rect.left()));
  KDCoordinate right = std::round(floatToPixel(Axis::Horizontal, rect.right()));
  KDCoordinate top = std::round(floatToPixel(Axis::Vertical, rect.top()));
  KDCoordinate bottom = std::round(floatToPixel(Axis::Vertical, rect.bottom()));
  KDRect pixelRect(
    left,
    top,
    std::max(right - left, 1), // Rectangle should at least be visible
    bottom - top
  );
  ctx->fillRect(pixelRect, rect.color());
}

void PlotView::traceLabel(KDContext * ctx, KDRect r, PlotStore::Label label) const {
  drawLabel(ctx, r,
    label.x(), label.y(), label.string(),
    KDColorBlack,
    RelativePosition::After,
    RelativePosition::After
  );
}

}
