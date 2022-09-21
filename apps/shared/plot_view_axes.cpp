#include "plot_view_axes.h"
#include <poincare/print.h>
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

// WithPolarGrid

void WithPolarGrid::drawGrid(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  constexpr float minimumGraduationDistanceToCenter = 60;
  constexpr int angleStepInDegree = 15;
  // The widest label is of length 4 : '360°'
  const int graduationHorizontalMargin = KDFont::GlyphWidth(KDFont::Size::Small) * 4 / 2;
  const int graduationVerticalMargin = KDFont::GlyphHeight(KDFont::Size::Small) / 2;
  constexpr int bufferSize = sizeof("360°");
  char buffer[bufferSize];
  float xMin = plotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, rect.left() + graduationHorizontalMargin);
  float xMax = plotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, rect.right() - graduationHorizontalMargin);
  float yMin = plotView->pixelToFloat(AbstractPlotView::Axis::Vertical, rect.bottom() - graduationVerticalMargin);
  float yMax = plotView->pixelToFloat(AbstractPlotView::Axis::Vertical, rect.top() + graduationVerticalMargin);
  float length = 2.f * std::max({-xMin, xMax, -yMin, yMax});
  for (int angle = 0; angle <= 360; angle += angleStepInDegree) {
    float angleRadian = angle *  M_PI / 180.f;
    float cos = std::cos(angleRadian);
    float sin = std::sin(angleRadian);
    bool shouldHaveGraduation = angle % (2 * angleStepInDegree) == 0;
    // drawSegment(ctx, rect, 0.f, 0.f, length * cos, length * sin, shouldHaveGraduation ? k_boldColor : k_lightColor, false);
    KDPoint from = KDPoint(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, 0.f), plotView->floatToPixel(AbstractPlotView::Axis::Vertical, 0.f));
    KDPoint to = KDPoint(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, length * cos), plotView->floatToPixel(AbstractPlotView::Axis::Vertical, length * sin));
    if (angle % 90) {
      ctx->drawAntialiasedLine(from, to, shouldHaveGraduation ? k_boldColor : k_lightColor, KDColorWhite);
    }
    if (!shouldHaveGraduation) {
      continue;
    }
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%i°", angle);
    // Compute the position of the label
    float x, y;
    AbstractPlotView::RelativePosition verticalRelativePosition = AbstractPlotView::RelativePosition::There;
    AbstractPlotView::RelativePosition horizontalRelativePosition = AbstractPlotView::RelativePosition::There;
    float xQuadrant = cos >= 0 ? xMax : xMin;
    float yQuadrant = sin >= 0 ? yMax : yMin;
    if (std::abs(cos)*std::abs(yQuadrant) > std::abs(sin)*std::abs(xQuadrant)) {
      float marginToRemove = AbstractPlotView::k_labelMargin * plotView->pixelHeight();//LengthToFloatLength(AbstractPlotView::Axis::Vertical, k_labelMargin);
      x = xQuadrant;
      y = xQuadrant * sin/cos + (angle == 360 ? 0 : -marginToRemove);
      if (angle % 180 == 0) {
        // Since the axis is black, we need to draw the graduation under or on top of it
        verticalRelativePosition = angle == 360 ? AbstractPlotView::RelativePosition::Before : AbstractPlotView::RelativePosition::After;
      }
    } else {
      x = std::copysign(yQuadrant * cos/sin, cos);
      y = yQuadrant;
      if (angle == 90) {
        horizontalRelativePosition = AbstractPlotView::RelativePosition::After;
      }
    }
    // TODO: update condition for non-orthonormal axes
    if (x*x + y*y > std::pow(minimumGraduationDistanceToCenter * plotView->pixelWidth(), 2)) {//plotView->pixelLengthToFloatLength(AbstractPlotView::Axis::Horizontal, minimumGraduationDistanceToCenter),2)) {
      plotView->drawLabel(ctx, rect, buffer, {x, y}, horizontalRelativePosition, verticalRelativePosition, k_boldColor);
    }
    /* TODO: don't print labels if the origin is off-screen and they end up on
     * the other side */
  }
}


// PlainAxis

void PlainAxis::drawAxis(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis) const {
  assert(plotView);
  plotView->drawStraightSegment(ctx, rect, axis, 0.f, -INFINITY, INFINITY, k_color);
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

KDRect AbstractLabeledAxis::labelRect(int i, float t, const AbstractPlotView * plotView, AbstractPlotView::Axis axis) const {
  assert(i < numberOfLabels());
  assert(t >= plotView->rangeMin(axis));

  const char * text = label(i);
  if (m_hidden || text[0] == '\0') {
    return KDRectZero;
  }

  if (i == 0) {
    computeLabelsRelativePosition(plotView, axis);
  }
  AbstractPlotView::RelativePosition xRelative, yRelative;
  if (axis == AbstractPlotView::Axis::Horizontal) {
    if (t == 0.f && m_otherAxis) {
      if (m_labelsPosition != 0.f) {
        /* Do not draw a floating 0 label. */
        return KDRectZero;
      }
      /* 0 must not be overlaid on the vertical axis. */
      xRelative = AbstractPlotView::RelativePosition::Before;
    } else {
      xRelative = AbstractPlotView::RelativePosition::There;
    }
    yRelative = m_relativePosition;
  } else {
    if (t == 0.f && m_otherAxis) {
      return KDRectZero;
    }
    xRelative = m_relativePosition;
    yRelative = AbstractPlotView::RelativePosition::There;
  }

  Coordinate2D<float> xy = axis == AbstractPlotView::Axis::Horizontal ? Coordinate2D<float>(t, m_labelsPosition) : Coordinate2D<float>(m_labelsPosition, t);
  return plotView->labelRect(text, xy, xRelative, yRelative);
}

void AbstractLabeledAxis::drawLabel(int i, float t, const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis axis, KDColor color) const {
  const char * text = label(i);
  KDRect thisLabelRect = labelRect(i, t, plotView, axis);
  if (thisLabelRect.intersects(rect) && labelWillBeDisplayed(i, thisLabelRect)) {
    plotView->drawLabel(ctx, rect, text, thisLabelRect, color);
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
