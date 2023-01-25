#include "plot_curve_view.h"
#include "plot_controller.h"
#include <algorithm>
#include <assert.h>

using namespace Poincare;
using namespace Shared;

namespace Statistics {

// LabeledAxisWithOptionalPercent

float LabeledAxisWithOptionalPercent::tickStep(const Shared::AbstractPlotView * plotView, Shared::AbstractPlotView::Axis axis) const {
  return PlotPolicy::VerticalLabeledAxis::tickStep(plotView, axis) * m_plotController->labelStepMultiplicator(axis);
}

int LabeledAxisWithOptionalPercent::computeLabel(int i, const Shared::AbstractPlotView * plotView, AbstractPlotView::Axis axis) {
  int length = PlotPolicy::VerticalLabeledAxis::computeLabel(i, plotView, axis);
  m_plotController->appendLabelSuffix(axis, &m_labels[i][length], k_labelBufferMaxSize - length, length, k_labelBufferMaxGlyphLength - length);
  return k_labelBufferMaxSize;
}

// PlotViewPolicy

void PlotViewPolicy::drawPlot(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  int selectedSeries = m_plotController->selectedSeries();
  for (int i = 0; i < Store::k_numberOfSeries; i++) {
    if (selectedSeries != i && m_plotController->seriesIsActive(i)) {
      drawSeriesCurve(plotView, ctx, rect, i);
    }
  }
  // Draw the selected curve last so that it's over the others
  assert(m_plotController->seriesIsActive(selectedSeries));
  drawSeriesCurve(plotView, ctx, rect, selectedSeries);
}

void PlotViewPolicy::drawSeriesCurve(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, int series) const {
  int numberOfPairs = m_plotController->totalValues(series);
  // Draw and connect each points
  KDColor color = Store::colorOfSeriesAtIndex(series);
  double previousX = NAN, previousY = NAN;
  for (int i = 0; i < numberOfPairs; i++) {
    double x = m_plotController->valueAtIndex(series, i);
    double y = m_plotController->resultAtIndex(series, i);
    plotView->drawDot(ctx, rect, Dots::Size::Tiny, Coordinate2D<float>(x, y), color);
    if (m_plotController->connectPoints() && i > 0) {
      plotView->drawSegment(ctx, rect, Coordinate2D<float>(x, y), Coordinate2D<float>(previousX, previousY), color, true);
    }
    previousX = x;
    previousY = y;
  }
  float x, y, u, v;
  if (m_plotController->drawSeriesZScoreLine(series, &x, &y, &u, &v)) {
    // Using brighter colors for the lines
    color = Store::colorLightOfSeriesAtIndex(series);
    plotView->drawSegment(ctx, rect, Coordinate2D<float>(x, y), Coordinate2D<float>(u, v), color, true);
  }
}

// PlotCurveView

PlotCurveView::PlotCurveView(Shared::CurveViewRange * range, Shared::CurveViewCursor * cursor, Shared::CursorView * cursorView, PlotController * plotController) :
  PlotView(range)
{
  // PlotViewAxes
  m_yAxis.setPlotController(plotController);
  // PlotViewPolicy
  m_plotController = plotController;
  // PlotPolicy::WithCursor
  m_cursor = cursor;
  m_cursorView = cursorView;
}

}
