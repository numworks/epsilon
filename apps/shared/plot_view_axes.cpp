#include "plot_view_axes.h"
#include <limits.h>
#include <cmath>

using namespace Poincare;

namespace Shared {
namespace PlotPolicy {

// WithGrid

void WithGrid::drawGrid(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  drawGridLines(plotView, ctx, rect, AbstractPlotView::Axis::Vertical, false);
  drawGridLines(plotView, ctx, rect, AbstractPlotView::Axis::Horizontal, false);
  drawGridLines(plotView, ctx, rect, AbstractPlotView::Axis::Vertical, true);
  drawGridLines(plotView, ctx, rect, AbstractPlotView::Axis::Horizontal, true);
}

void WithGrid::drawGridLines(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis parallel, bool boldGrid) const {
  KDColor color = boldGrid ? k_boldColor : k_lightColor;
  bool oddIndexes = !boldGrid;
  assert(plotView);
  float min, max, step;
  CurveViewRange * range = plotView->range();
  if (parallel == AbstractPlotView::Axis::Horizontal) {
    min = range->yMin();
    max = range->yMax();
    step = range->yGridUnit();
  } else {
    min = range->xMin();
    max = range->xMax();
    step = range->xGridUnit();
  }

  int iMin = min / step;
  int iMax = max / step;
  for (int i = iMin + ((iMin % 2 == 0) == oddIndexes); i <= iMax; i += 2) {
    plotView->drawStraightSegment(ctx, rect, parallel, i * step, -INFINITY, INFINITY, color);
  }
}

// SimpleAxis

void SimpleAxis::drawAxis(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis) const {
  assert(plotView);

  // - Draw plain axis
  plotView->drawStraightSegment(ctx, rect, axis, 0.f, -INFINITY, INFINITY, k_color);

  // - Draw ticks and eventual labels
  /* Do not draw ticks on the vertical axis if the axis itself is not visible,
   * as they could be mistaken for minus signs. */
  bool drawTicks = !(axis == AbstractPlotView::Axis::Vertical && plotView->range()->xMin() >= 0.f);
  float tMax = plotView->rangeMax(axis) + plotView->pixelLength(axis);
  int i = 0;
  float t = tickPosition(i, plotView, axis);
  while (t <= tMax) {
    if (drawTicks) {
      plotView->drawTick(ctx, rect, axis, t, k_color);
    }
    drawLabel(i, t, plotView, ctx, rect, axis);
    i++;
    t = tickPosition(i, plotView, axis);
  }
}

float SimpleAxis::tickPosition(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const {
  float step = tickStep(plotView, axis);
  float tMin = plotView->rangeMin(axis);
  assert(std::fabs(std::round(tMin / step)) < INT_MAX);
  int indexOfOrigin = std::floor(-tMin / step);
  return step * (i - indexOfOrigin);
}

float SimpleAxis::tickStep(const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const {
  float step = axis == AbstractPlotView::Axis::Horizontal ? plotView->range()->xGridUnit() : plotView->range()->yGridUnit();
  return 2.f * step;
}

// AbstractLabeledAxis

void AbstractLabeledAxis::reloadAxis(AbstractPlotView * plotView, AbstractPlotView::Axis axis) {
  size_t n = numberOfLabels();
  for (size_t i = 0; i < n; i++) {
    computeLabel(i, plotView, axis);
  }
}

int AbstractLabeledAxis::computeLabel(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) {
  float t = tickPosition(i, plotView, axis);
  return Poincare::PrintFloat::ConvertFloatToText(t, mutableLabel(i), k_labelBufferMaxSize, k_labelBufferMaxGlyphLength, k_numberSignificantDigits, Preferences::PrintFloatMode::Decimal).GlyphLength;
}

void AbstractLabeledAxis::drawLabel(int i, float t, const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis, KDColor color) const {
  assert(i < numberOfLabels());
  assert(t >= plotView->rangeMin(axis));

  const char * text = label(i);
  if (m_hidden || text[0] == '\0') {
    return;
  }

  if (i == 0) {
    computeLabelsRelativePosition(plotView, axis);
  }
  AbstractPlotView::RelativePosition xRelative, yRelative;
  if (axis == AbstractPlotView::Axis::Horizontal) {
    if (t == 0.f && m_otherAxis) {
      if (m_labelsPosition != 0.f) {
        /* Do not draw a floating 0 label. */
        return;
      }
      /* 0 must not be overlaid on the vertical axis. */
      xRelative = AbstractPlotView::RelativePosition::Before;
    } else {
      xRelative = AbstractPlotView::RelativePosition::There;
    }
    yRelative = m_relativePosition;
  } else {
    if (t == 0.f && m_otherAxis) {
      return;
    }
    xRelative = m_relativePosition;
    yRelative = AbstractPlotView::RelativePosition::There;
  }

  Coordinate2D<float> xy = axis == AbstractPlotView::Axis::Horizontal ? Coordinate2D<float>(t, m_labelsPosition) : Coordinate2D<float>(m_labelsPosition, t);
  KDRect labelRect = plotView->labelRect(text, xy, xRelative, yRelative);
  if (labelWillBeDisplayed(i, labelRect)) {
    plotView->drawLabel(ctx, rect, text, labelRect, color);
  }
}

void AbstractLabeledAxis::computeLabelsRelativePosition(const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const {
  m_labelsPosition = 0.f;

  if (axis == AbstractPlotView::Axis::Horizontal) {
    float labelHeight = (KDFont::GlyphSize(AbstractPlotView::k_font).height() + AbstractPlotView::k_labelMargin) * plotView->pixelHeight();
    float bannerHeight = plotView->bannerView() && plotView->bannerOverlapsGraph() ? plotView->bannerView()->bounds().height() * plotView->pixelHeight() : 0.f;
    float yMin = plotView->range()->yMin();
    float yMax = plotView->range()->yMax();
    if (yMin + bannerHeight > 0.f - labelHeight) {
      m_relativePosition = AbstractPlotView::RelativePosition::Before;
      m_labelsPosition = yMin + bannerHeight;
    } else {
      if (yMax < 0.f) {
        m_labelsPosition = yMax;
      }
      m_relativePosition = AbstractPlotView::RelativePosition::After;
    }
  } else {
    KDCoordinate labelsWidth = 0;
    int n = numberOfLabels();
    for (int i = 0; i < n; i++) {
      KDCoordinate w = KDFont::Font(AbstractPlotView::k_font)->stringSize(label(i)).width();
      if (w > labelsWidth) {
        labelsWidth = w;
      }
    }
    float xMin = plotView->range()->xMin();
    float xMax = plotView->range()->xMax();
    if (xMin  + labelsWidth * plotView->pixelWidth() > 0.f) {
      m_relativePosition = AbstractPlotView::RelativePosition::After;
      m_labelsPosition = std::max(xMin, 0.f);
    } else {
      if (xMax < 0.f) {
        m_labelsPosition = xMax;
      }
      m_relativePosition = AbstractPlotView::RelativePosition::Before;
    }
  }
}

}
}
