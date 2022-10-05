#include "plot_view_axes.h"
#include <cmath>

using namespace Poincare;

namespace Shared {
namespace PlotPolicy {

// WithGrid

void WithGrid::drawGrid(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  drawGridLines(plotView, ctx, rect, AbstractPlotView::Axis::Vertical);
  drawGridLines(plotView, ctx, rect, AbstractPlotView::Axis::Horizontal);
}

void WithGrid::drawGridLines(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis parallel) const {
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

  int iMax = max / step;
  for (int i = min / step; i <= iMax; i++) {
    plotView->drawStraightSegment(ctx, rect, parallel, i * step, -INFINITY, INFINITY, i % 2 == 0 ? k_boldColor : k_lightColor);
  }
}

// SimpleAxis

void SimpleAxis::drawAxis(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis) const {
  assert(plotView);

  AbstractPlotView::Axis otherAxis = AbstractPlotView::OtherAxis(axis);
  float graduationFloatLength = k_labelGraduationHalfLength * plotView->pixelLength(otherAxis);

  // - Draw plain axis
  plotView->drawStraightSegment(ctx, rect, axis, 0.f, -INFINITY, INFINITY, k_color);

  // - Draw ticks and eventual labels
  /* Do not draw ticks on the vertical axis if the axis itslef is not visible,
   * as they could be mistaken for minus signs. */
  bool drawTicks = !(axis ==  AbstractPlotView::Axis::Vertical && plotView->range()->xMin() >= 0.f);
  float tMax = plotView->rangeMax(axis);
  int i = 0;
  float t = tickPosition(i, plotView, axis);
  while (t <= tMax) {
    if (drawTicks) {
      plotView->drawStraightSegment(ctx, rect, otherAxis, t, -graduationFloatLength, graduationFloatLength, k_color);
    }
    drawLabel(i, t, plotView, ctx, rect, axis);
    i++;
    t = tickPosition(i, plotView, axis);
  }
}

float SimpleAxis::tickPosition(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const {
  float step = tickStep(plotView, axis);
  float tMin = axis == AbstractPlotView::Axis::Horizontal ? plotView->range()->xMin() : plotView->range()->yMin();
  assert(std::fabs(std::round(tMin / step)) < INT_MAX);
  int indexOfOrigin = std::round(-tMin / step);
  return step * (i - indexOfOrigin);
}

float SimpleAxis::tickStep(const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const {
  float step = axis == AbstractPlotView::Axis::Horizontal ? plotView->range()->xGridUnit() : plotView->range()->yGridUnit();
  return 2.f * step;
}

// LabeledAxis

void LabeledAxis::reloadAxis(AbstractPlotView * plotView, AbstractPlotView::Axis axis) {
  for (size_t i = 0; i < k_maxNumberOfLabels; i++) {
    computeLabel(i, plotView, axis);
  }
}

void LabeledAxis::forceRelativePosition(AbstractPlotView::RelativePosition position) {
  m_forceRelativePosition = true;
  m_relativePosition = position;
}

int LabeledAxis::computeLabel(int i, const AbstractPlotView * plotView, AbstractPlotView::Axis axis)  {
  float t = tickPosition(i, plotView, axis);
  return Poincare::PrintFloat::ConvertFloatToText(t, m_labels[i], k_labelBufferMaxSize, k_labelBufferMaxGlyphLength, k_numberSignificantDigits, Preferences::PrintFloatMode::Decimal).GlyphLength;
}

void LabeledAxis::drawLabel(int i, float t, const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis) const {
  assert(i < k_maxNumberOfLabels);
  const char * text = m_labels[i];
  if (m_hidden || text[0] == '\0') {
    return;
  }

  if (i == 0) {
    computeLabelsRelativePosition(plotView, axis);
  }
  AbstractPlotView::RelativePosition xRelative, yRelative;
  if (axis == AbstractPlotView::Axis::Horizontal) {
    if (t == 0.f && m_labelsPosition != 0.f) {
        return;
    } else if (t == 0.f && m_offsetOrigin) {
      xRelative = AbstractPlotView::RelativePosition::Before;
    } else {
      xRelative = AbstractPlotView::RelativePosition::There;
    }
    yRelative = m_relativePosition;
  } else {
    if (t == 0.f) {
      return;
    }
    xRelative = m_relativePosition;
    yRelative = AbstractPlotView::RelativePosition::There;
  }

  Coordinate2D<float> xy = axis == AbstractPlotView::Axis::Horizontal ? Coordinate2D<float>(t, m_labelsPosition) : Coordinate2D<float>(m_labelsPosition, t);
  plotView->drawLabel(ctx, rect, text, xy, xRelative, yRelative, k_color);
}

void LabeledAxis::computeLabelsRelativePosition(const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const {
  m_labelsPosition = 0.f;
  if (m_forceRelativePosition) {
    return;
  }

  if (axis == AbstractPlotView::Axis::Horizontal) {
    KDCoordinate labelHeight = (KDFont::GlyphSize(AbstractPlotView::k_font).height() + 2 * AbstractPlotView::k_labelMargin) * plotView->pixelHeight();
    float bannerHeight = plotView->bannerView() ? plotView->bannerView()->bounds().height() * plotView->pixelHeight() : 0.f;
    float yMin = plotView->range()->yMin();
    float yMax = plotView->range()->yMax();
    if (yMin > - bannerHeight - labelHeight) {
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
    for (int i = 0; i < k_maxNumberOfLabels; i++) {
      KDCoordinate w = KDFont::Font(AbstractPlotView::k_font)->stringSize(m_labels[i]).width();
      if (w > labelsWidth) {
        labelsWidth = w;
      }
    }
    float xMin = plotView->range()->xMin();
    float xMax = plotView->range()->xMax();
    if (xMin > - labelsWidth * plotView->pixelWidth()) {
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
