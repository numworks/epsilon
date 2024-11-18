#include "plot_view_axes.h"

#include <limits.h>
#include <poincare/print.h>

#include <cmath>

using namespace Poincare;

namespace Shared {
namespace PlotPolicy {

// WithCartesianGrid

void WithCartesianGrid::DrawGrid(const AbstractPlotView* plotView,
                                 KDContext* ctx, KDRect rect) {
  DrawGridLines(plotView, ctx, rect, OMG::Axis::Vertical, false);
  DrawGridLines(plotView, ctx, rect, OMG::Axis::Horizontal, false);
  DrawGridLines(plotView, ctx, rect, OMG::Axis::Vertical, true);
  DrawGridLines(plotView, ctx, rect, OMG::Axis::Horizontal, true);
}

void WithCartesianGrid::DrawGridLines(const AbstractPlotView* plotView,
                                      KDContext* ctx, KDRect rect,
                                      OMG::Axis parallel, bool boldGrid) {
  KDColor color = boldGrid ? k_boldColor : k_lightColor;
  bool oddIndexes = !boldGrid;
  assert(plotView);
  float min, max, step;
  CurveViewRange* range = plotView->range();
  if (parallel == OMG::Axis::Horizontal) {
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
    plotView->drawStraightSegment(ctx, rect, parallel, i * step, -INFINITY,
                                  INFINITY, color);
  }
}

// WithPolarGrid

void WithPolarGrid::ComputeRadiusBounds(const AbstractPlotView* plotView,
                                        KDRect rect, float* radiusMin,
                                        float* radiusMax) {
  /* We translate the pixel coordinates into floats, adding/subtracting 1 to
   * account for conversion errors. */
  float xMin = plotView->pixelToFloat(OMG::Axis::Horizontal, rect.left() - 1);
  float xMax = plotView->pixelToFloat(OMG::Axis::Horizontal, rect.right() + 1);
  float yMin = plotView->pixelToFloat(OMG::Axis::Vertical, rect.bottom() + 1);
  float yMax = plotView->pixelToFloat(OMG::Axis::Vertical, rect.top() - 1);

  float xAbsoluteMax = std::max(-xMin, xMax);
  float yAbsoluteMax = std::max(-yMin, yMax);
  float xAbsoluteMin = xMin <= 0 && xMax >= 0 ? 0 : std::min(-xMin, xMax);
  float yAbsoluteMin = yMin <= 0 && yMax >= 0 ? 0 : std::min(-yMin, yMax);

  *radiusMin = sqrt(xAbsoluteMin * xAbsoluteMin + yAbsoluteMin * yAbsoluteMin);
  *radiusMax = sqrt(xAbsoluteMax * xAbsoluteMax + yAbsoluteMax * yAbsoluteMax);
}

void WithPolarGrid::ComputeAngleBounds(const AbstractPlotView* plotView,
                                       KDRect rect, float xMin, float xMax,
                                       float yMin, float yMax, float* angleMin,
                                       float* angleMax) {
  float xAbsoluteMax = xMax > -xMin ? xMax : xMin;
  float yAbsoluteMax = yMax > -yMin ? yMax : yMin;
  float xAbsoluteMin = xMin > -xMax ? xMin : xMax;
  float yAbsoluteMin = yMin > -yMax ? yMin : yMax;

  if (xMin <= 0 && 0 <= xMax && yMin <= 0 && 0 <= yMax) {
    // The rect contains the origin.
    *angleMin = 0;
    *angleMax = 2 * M_PI;
  } else if (xMin <= 0 && 0 <= xMax) {
    // The rect crosses the Y Axis.
    *angleMin = atan2(yAbsoluteMin, xMin);
    *angleMax = atan2(yAbsoluteMin, xMax);
  } else if (yMin <= 0 && 0 <= yMax) {
    // The rect crosses the X axis.
    *angleMin = atan2(yMin, xAbsoluteMin);
    *angleMax = atan2(yMax, xAbsoluteMin);

    if (xMax < 0) {
      /* The rect crosses the -Ox ray. As atan2 outputs in the range [-pi,pi],
       * the [a0, a1] arc must be complemented. */
      *angleMin += 2.f * M_PI;
    }
  } else {
    *angleMin = atan2(yAbsoluteMin, xAbsoluteMax);
    *angleMax = atan2(yAbsoluteMax, xAbsoluteMin);
  }

  if (*angleMin > *angleMax) {
    std::swap(*angleMin, *angleMax);
  }
}

void WithPolarGrid::DrawPolarCircles(const AbstractPlotView* plotView,
                                     KDContext* ctx, KDRect rect) {
  float radiusMin, radiusMax;

  ComputeRadiusBounds(plotView, rect, &radiusMin, &radiusMax);

  float step = plotView->range()->xGridUnit();

  for (int i = std::max<int>(1, std::floor(radiusMin / step));
       i <= std::ceil(radiusMax / step); i++) {
    plotView->drawCircle(ctx, rect, {0.f, 0.f}, i * step,
                         i % 2 ? k_lightColor : k_boldColor);
  }
}

void WithPolarGrid::DrawGrid(const AbstractPlotView* plotView, KDContext* ctx,
                             KDRect rect) {
  DrawPolarCircles(plotView, ctx, rect);
  // The widest label is of length 4 : '360°'
  constexpr int graduationHorizontalMargin =
      KDFont::GlyphWidth(KDFont::Size::Small) * 4 / 2;
  constexpr int graduationVerticalMargin =
      KDFont::GlyphHeight(KDFont::Size::Small) / 2;
  constexpr int bufferSize = sizeof("360°");
  char buffer[bufferSize];
  KDRect bounds = plotView->bounds();
  float xMin = plotView->pixelToFloat(
      OMG::Axis::Horizontal, bounds.left() + graduationHorizontalMargin);
  float xMax = plotView->pixelToFloat(
      OMG::Axis::Horizontal, bounds.right() - graduationHorizontalMargin);
  float yMin = plotView->pixelToFloat(
      OMG::Axis::Vertical, bounds.bottom() - graduationVerticalMargin);
  float yMax = plotView->pixelToFloat(OMG::Axis::Vertical,
                                      bounds.top() + graduationVerticalMargin);

  float bannerHeight =
      plotView->bannerView() && plotView->bannerOverlapsGraph()
          ? plotView->bannerView()->bounds().height() * plotView->pixelHeight()
          : 0.f;
  yMin += bannerHeight;

  float labelMargin = AbstractPlotView::k_labelMargin * plotView->pixelHeight();
  float minimumSquareDistanceToCenter =
      std::pow(k_minimumGraduationDistanceToCenter * plotView->pixelWidth(), 2);

  // Find angle bounds.
  float angleBegin;
  float angleEnd;
  float radiusMin, radiusMax;

  ComputeRadiusBounds(plotView, rect, &radiusMin, &radiusMax);
  ComputeAngleBounds(plotView, rect, xMin, xMax, yMin, yMax, &angleBegin,
                     &angleEnd);

  float k_angleStep = k_angleStepInDegree * M_PI / 180;

  for (int i = std::floor(angleBegin / k_angleStep);
       i <= std::ceil(angleEnd / k_angleStep); i++) {
    // Little trick to handle negative angles.
    int angle = (i * k_angleStepInDegree + 360) % 360;

    float angleRadian = angle * M_PI / 180.f;
    float cos = std::cos(angleRadian);
    float sin = std::sin(angleRadian);
    bool shouldHaveGraduation = angle % (2 * k_angleStepInDegree) == 0;
    // Don't draw under the axes.
    if (angle % 90) {
      KDPoint pixelFrom = {plotView->floatToKDCoordinatePixel(
                               OMG::Axis::Horizontal, cos * radiusMin),
                           plotView->floatToKDCoordinatePixel(
                               OMG::Axis::Vertical, sin * radiusMin)};
      KDPoint pixelTo = {plotView->floatToKDCoordinatePixel(
                             OMG::Axis::Horizontal, cos * radiusMax),
                         plotView->floatToKDCoordinatePixel(OMG::Axis::Vertical,
                                                            sin * radiusMax)};
      ctx->drawAntialiasedLine(
          pixelFrom, pixelTo, shouldHaveGraduation ? k_boldColor : k_lightColor,
          plotView->k_backgroundColor);
    }
    if (!shouldHaveGraduation) {
      continue;
    }
    Poincare::Print::CustomPrintf(buffer, bufferSize, "%i°", angle);
    // Compute the position of the label
    float x, y;
    AbstractPlotView::RelativePosition verticalRelativePosition =
        AbstractPlotView::RelativePosition::There;
    AbstractPlotView::RelativePosition horizontalRelativePosition =
        AbstractPlotView::RelativePosition::There;
    float xQuadrant = cos >= 0 ? xMax : xMin;
    float yQuadrant = sin >= 0 ? yMax : yMin;
    // Is the ray exiting the screen through a side ?
    if (std::fabs(cos) * std::fabs(yQuadrant) >
        std::fabs(sin) * std::fabs(xQuadrant)) {
      x = xQuadrant;
      y = xQuadrant * sin / cos + (angle == 360 ? 0 : -labelMargin);
      if (angle % 180 == 0) {
        /* Since the axis is black, we need to draw the graduation under or on
         * top of it */
        verticalRelativePosition =
            angle == 360 ? AbstractPlotView::RelativePosition::After
                         : AbstractPlotView::RelativePosition::Before;
      }
    } else {
      // The ray is exiting through the top or the bottom
      x = std::copysign(yQuadrant * cos / sin, cos);
      y = yQuadrant;
      if (angle == 90 || angle == 270) {
        // Move the label for it not to be overriden by the vertical axis.
        horizontalRelativePosition = AbstractPlotView::RelativePosition::After;
      }
    }
    // TODO: update condition for non-orthonormal axes
    if (x * x + y * y > minimumSquareDistanceToCenter) {
      plotView->drawLabel(ctx, rect, buffer, {x, y}, horizontalRelativePosition,
                          verticalRelativePosition, k_boldColor);

      if (angle == 0) {
        // Show 360° as well
        plotView->drawLabel(
            ctx, rect, "360°", {x, y + labelMargin}, horizontalRelativePosition,
            AbstractPlotView::RelativePosition::After, k_boldColor);
      }
    }
    /* TODO: don't print labels if the origin is off-screen and they end up on
     * the other side */
  }
}

// PlainAxis

void PlainAxis::drawAxis(const AbstractPlotView* plotView, KDContext* ctx,
                         KDRect rect, OMG::Axis axis) const {
  assert(plotView);
  plotView->drawStraightSegment(ctx, rect, axis, 0.f, -INFINITY, INFINITY,
                                k_color);
}

// SimpleAxis

void SimpleAxis::drawAxis(const AbstractPlotView* plotView, KDContext* ctx,
                          KDRect rect, OMG::Axis axis) const {
  assert(plotView);

  // - Draw plain axis
  PlainAxis::drawAxis(plotView, ctx, rect, axis);

  // - Draw ticks and eventual labels
  /* Do not draw ticks on the vertical axis if the axis itself is not visible,
   * as they could be mistaken for minus signs. */
  bool drawTicks =
      !(axis == OMG::Axis::Vertical && plotView->range()->xMin() >= 0.f);
  float tMax = plotView->rangeMax(axis) + plotView->pixelLength(axis);
  int i = 0;
  float t = tickPosition(i, plotView, axis);
  /* maxNumberOfTicks is there to ensure that we do not draw too much ticks
   * when reaching limit cases. For example, tMax = 1.0E+8 and tickStep = 0.5,
   * when t == tMax, increasing if by tickstep will give:
   *  t = tMax + tickStep, but tMax + tickStep == tMax because of float
   * precision. Which means the condition t <= tMax in the loop will never be
   * reached. */
  int maxNumberOfTicks =
      std::ceil((tMax - plotView->rangeMin(axis)) / tickStep(plotView, axis)) +
      1;
  assert(maxNumberOfTicks >= 2);
  while (t < tMax && i < maxNumberOfTicks) {
    if (drawTicks) {
      plotView->drawTick(ctx, rect, axis, t, k_color);
    }
    drawLabel(i, t, plotView, ctx, rect, axis);
    i++;
    t = tickPosition(i, plotView, axis);
  }
}

float SimpleAxis::tickPosition(int i, const AbstractPlotView* plotView,
                               OMG::Axis axis) const {
  float step = tickStep(plotView, axis);
  float tMin = plotView->rangeMin(axis);
  assert(std::fabs(std::round(tMin / step)) < static_cast<float>(INT_MAX));
  int indexOfOrigin = std::floor(-tMin / step);
  return step * (i - indexOfOrigin);
}

float SimpleAxis::tickStep(const AbstractPlotView* plotView,
                           OMG::Axis axis) const {
  float step = axis == OMG::Axis::Horizontal ? plotView->range()->xGridUnit()
                                             : plotView->range()->yGridUnit();
  return 2.f * step;
}

// AbstractLabeledAxis

void AbstractLabeledAxis::reloadAxis(AbstractPlotView* plotView,
                                     OMG::Axis axis) {
  size_t n = numberOfLabels();
  m_lastDrawnRect = KDRectZero;
  for (size_t i = 0; i < n; i++) {
    computeLabel(i, plotView, axis);
  }
  computeLabelsRelativePosition(plotView, axis);
}

int AbstractLabeledAxis::computeLabel(int i, const AbstractPlotView* plotView,
                                      OMG::Axis axis) {
  float t = tickPosition(i, plotView, axis);
  return Poincare::PrintFloat::ConvertFloatToText(
             t, mutableLabel(i), k_labelBufferMaxSize,
             k_labelBufferMaxGlyphLength, k_numberSignificantDigits,
             Preferences::PrintFloatMode::Decimal)
      .GlyphLength;
}

bool AbstractLabeledAxis::labelWillBeDisplayed(int i, KDRect rect) const {
  return !rect.intersects(m_lastDrawnRect);
}

KDRect AbstractLabeledAxis::labelRect(int i, float t,
                                      const AbstractPlotView* plotView,
                                      OMG::Axis axis) const {
  assert(static_cast<size_t>(i) < numberOfLabels());

  const char* text = label(i);
  if (m_hidden || text[0] == '\0') {
    return KDRectZero;
  }

  AbstractPlotView::RelativePosition xRelative, yRelative;
  if (axis == OMG::Axis::Horizontal) {
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

  Coordinate2D<float> xy = axis == OMG::Axis::Horizontal
                               ? Coordinate2D<float>(t, m_labelsPosition)
                               : Coordinate2D<float>(m_labelsPosition, t);
  return plotView->labelRect(text, xy, xRelative, yRelative);
}

void AbstractLabeledAxis::drawLabel(int i, float t,
                                    const AbstractPlotView* plotView,
                                    KDContext* ctx, KDRect rect, OMG::Axis axis,
                                    KDColor color) const {
  const char* text = label(i);
  KDRect thisLabelRect = labelRect(i, t, plotView, axis);
  if (thisLabelRect.intersects(rect) &&
      labelWillBeDisplayed(i, thisLabelRect)) {
    m_lastDrawnRect = thisLabelRect.paddedWith(AbstractPlotView::k_labelMargin);
    plotView->drawLabel(ctx, rect, text, thisLabelRect, color);
  }
}

void AbstractLabeledAxis::computeLabelsRelativePosition(
    const AbstractPlotView* plotView, OMG::Axis axis) const {
  m_labelsPosition = 0.f;

  if (axis == OMG::Axis::Horizontal) {
    float labelHeight = (KDFont::GlyphSize(AbstractPlotView::k_font).height() +
                         AbstractPlotView::k_labelMargin) *
                        plotView->pixelHeight();
    float bannerHeight =
        plotView->bannerView() && plotView->bannerOverlapsGraph()
            ? plotView->bannerView()->bounds().height() *
                  plotView->pixelHeight()
            : 0.f;
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
      KDCoordinate w =
          KDFont::Font(AbstractPlotView::k_font)->stringSize(label(i)).width();
      if (w > labelsWidth) {
        labelsWidth = w;
      }
    }
    float xMin = plotView->range()->xMin();
    float xMax = plotView->range()->xMax();
    if (xMin + labelsWidth * plotView->pixelWidth() > 0.f) {
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

}  // namespace PlotPolicy
}  // namespace Shared
