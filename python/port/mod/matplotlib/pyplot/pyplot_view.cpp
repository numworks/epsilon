#include "pyplot_view.h"
#include <algorithm>
#include <python/port/port.h>

using namespace Shared;
using namespace Poincare;

namespace Matplotlib {

// OptionalAxes

void OptionalAxes::drawAxesAndGrid(const Shared::AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  PlotStore * store = plotStore();
  if (store->gridRequested()) {
    m_grid.drawAxesAndGrid(plotView, ctx, rect);
  }
  if (store->axesRequested()) {
    m_axes.drawAxesAndGrid(plotView, ctx, rect);
  }
}

// PyplotPolicy

void PyplotPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  /* The following lines use MicroPython, which can fail, so we need to use nlr
   * to catch any errors. */
  nlr_buf_t nlr;
  if (nlr_push(&nlr) == 0) {
    for (PlotStore::Dot dot : m_store->dots()) {
      traceDot(plotView, ctx, rect, dot);
    }
    for (PlotStore::Label label : m_store->labels()) {
      traceLabel(plotView, ctx, rect, label);
    }
    for (PlotStore::Segment segment : m_store->segments()) {
      traceSegment(plotView, ctx, rect, segment);
    }
    for (PlotStore::Rect rectangle : m_store->rects()) {
      traceRect(plotView, ctx, rect, rectangle);
    }
    nlr_pop();
  } else { // Uncaught exception
    MicroPython::ExecutionEnvironment::HandleException(&nlr, m_micropythonEnvironment);
  }
}

void PyplotPolicy::traceDot(const AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Dot dot) const {
  plotView->drawDot(ctx, r, Dots::Size::Tiny, Poincare::Coordinate2D<float>(dot.x(), dot.y()), dot.color());
}

void PyplotPolicy::traceSegment(const AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Segment segment) const {
  plotView->drawSegment(ctx, r, Coordinate2D<float>(segment.xStart(), segment.yStart()), Coordinate2D<float>(segment.xEnd(), segment.yEnd()), segment.color(), true);
  if (!std::isnan(segment.arrowWidth())) {
    float dx = segment.xEnd() - segment.xStart();
    float dy = segment.yEnd() - segment.yStart();
    plotView->drawArrowhead(ctx, r, Coordinate2D<float>(segment.xEnd(), segment.yEnd()), Coordinate2D<float>(dx, dy), segment.arrowWidth(), segment.color(), true);
  }
}

void PyplotPolicy::traceRect(const AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Rect rect) const {
  KDCoordinate left =   std::round(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, rect.left()));
  KDCoordinate right =  std::round(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, rect.right()));
  KDCoordinate top =    std::round(plotView->floatToPixel(AbstractPlotView::Axis::Vertical, rect.top()));
  KDCoordinate bottom = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Vertical, rect.bottom()));
  KDRect pixelRect(
    left,
    top,
    std::max(right - left, 1), // Rectangle should at least be visible
    bottom - top
  );
  ctx->fillRect(pixelRect, rect.color());
}

void PyplotPolicy::traceLabel(const AbstractPlotView * plotView, KDContext * ctx, KDRect r, PlotStore::Label label) const {
  plotView->drawLabel(ctx, r, label.string(), Coordinate2D<float>(label.x(), label.y()), AbstractPlotView::RelativePosition::After, AbstractPlotView::RelativePosition::After, KDColorBlack);
}

// PyplotView

PyplotView::PyplotView(PlotStore * s) :
  PlotView(s)
{
  m_store = s;
}

}
