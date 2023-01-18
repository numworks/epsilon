#include "plot_view_plots.h"
#include "float.h"
#include <algorithm>

using namespace Poincare;

namespace Shared {
namespace PlotPolicy {

// WithCurves::Pattern

WithCurves::Pattern::Pattern(KDColor c0, KDColor c1, KDColor c2, KDColor c3, KDColor cBackground) :
  m_c0(c0),
  m_c1(c1),
  m_c2(c2),
  m_c3(c3),
  m_cBackground(cBackground)
{}

WithCurves::Pattern::Pattern(bool s0, bool s1, bool s2, bool s3, KDColor color, KDColor backgroundColor) :
  Pattern(s0 ? color : backgroundColor, s1 ? color : backgroundColor, s2 ? color : backgroundColor, s3 ? color : backgroundColor, backgroundColor)
{}

WithCurves::Pattern::Pattern(int s, KDColor color, KDColor backgroundColor) :
  Pattern(s == 0, s == 1, s == 2, s == 3, color, backgroundColor)
{}

void WithCurves::Pattern::drawInLine(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, AbstractPlotView::Axis parallel, float position, float min, float max) const {
  AbstractPlotView::Axis perpendicular = AbstractPlotView::OtherAxis(parallel);
  KDCoordinate posC = plotView->floatToKDCoordinatePixel(perpendicular, position);

  KDColor firstColor, secondColor;
  if (posC % (k_size / 2) == 0) {
    firstColor = m_c0;
    secondColor = m_c1;
  } else {
    firstColor = m_c2;
    secondColor = m_c3;
  }

  bool canSpeedUpDrawing = firstColor == m_cBackground && secondColor == m_cBackground;
  if (canSpeedUpDrawing) {
    if (m_cBackground != k_transparent) {
      plotView->drawStraightSegment(ctx, rect, parallel, position, min, max, m_cBackground);
    }
    return;
  }

  if (max < min) {
    std::swap(max, min);
  }
  KDCoordinate minC = plotView->floatToKDCoordinatePixel(parallel, min);
  KDCoordinate maxC = plotView->floatToKDCoordinatePixel(parallel, max);
  if (parallel == AbstractPlotView::Axis::Horizontal) {
    minC = std::max(minC, rect.left());
    maxC = std::min(maxC, static_cast<KDCoordinate>(rect.right() + 1));
  } else {
    /* Swap minC and maxC, as the Y axis changes direction between the range
     * space and screen space. */
    KDCoordinate temp = minC;
    minC = std::max(maxC, rect.top());
    maxC = std::min(temp, static_cast<KDCoordinate>(rect.bottom() + 1));
  }
  if (minC >= maxC) {
    return;
  }

  if (posC % k_size >= k_size / 2) {
    std::swap(firstColor, secondColor);
  }

  KDColor colors[k_size];
  if (posC % (k_size / 2) == 0) {
    colors[0] = firstColor;
    colors[2] = secondColor;
    colors[1] = colors[3] = m_cBackground;
  } else {
    colors[1] = firstColor;
    colors[3] = secondColor;
    colors[0] = colors[2] = m_cBackground;
  }

  for (int i = minC; i < maxC; i++) {
    KDColor color = colors[i % k_size];
    if (color != k_transparent) {
      ctx->setPixel(parallel == AbstractPlotView::Axis::Horizontal ? KDPoint(i, posC) : KDPoint(posC, i), color);
    }
  }
}

// WithCurves::CurveDrawing

WithCurves::CurveDrawing::CurveDrawing(Curve2D curve, void * context, float tStart, float tEnd, float tStep, KDColor color, bool thick, bool dashed) :
  m_curve(curve),
  m_pattern(),
  m_context(context),
  m_curveDouble(nullptr),
  m_discontinuity(NoDiscontinuity),
  m_tStart(tStart),
  m_tEnd(tEnd),
  m_tStep(tStep),
  m_patternStart(NAN),
  m_patternEnd(NAN),
  m_color(color),
  m_axis(AbstractPlotView::Axis::Horizontal),
  m_thick(thick),
  m_dashed(dashed),
  m_patternWithoutCurve(false),
  m_drawStraightLinesEarly(true)
{
  if (m_tStart > m_tEnd) {
    std::swap(m_tStart, m_tEnd);
  }
  // Assert that the chosen step is not too small (ad-hoc value)
  assert((m_tEnd - m_tStart) / m_tStep < 10e5);
}

void WithCurves::CurveDrawing::setPatternOptions(Pattern pattern, float patternStart, float patternEnd, Curve2D patternLowerBound, Curve2D patternUpperBound, bool patternWithoutCurve, AbstractPlotView::Axis axis) {
  m_pattern = pattern;
  m_patternStart = patternStart;
  m_patternEnd = patternEnd;
  m_patternLowerBound = patternLowerBound;
  m_patternUpperBound = patternUpperBound;
  m_patternWithoutCurve = patternWithoutCurve;
  m_axis = axis;
}

void WithCurves::CurveDrawing::setPrecisionOptions(bool drawStraightLinesEarly, Curve2DEvaluation<double> curveDouble, DiscontinuityTest discontinuity) {
  m_drawStraightLinesEarly = drawStraightLinesEarly;
  m_curveDouble = curveDouble;
  m_discontinuity = discontinuity;
}

void WithCurves::CurveDrawing::draw(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  assert(plotView);
  assert(m_tStart <= m_tEnd);

  plotView->setDashed(m_dashed);

  float previousT = NAN, t = NAN;
  Coordinate2D<float> previousXY, xy;
  float (Coordinate2D<float>::*abscissa)() const = m_axis == AbstractPlotView::Axis::Horizontal ? &Coordinate2D<float>::x1 : &Coordinate2D<float>::x2;
  float (Coordinate2D<float>::*ordinate)() const = m_axis == AbstractPlotView::Axis::Horizontal ? &Coordinate2D<float>::x2 : &Coordinate2D<float>::x1;
  int i = 0;
  bool isLastSegment = false;

  do {
    previousT = t;
    t = m_tStart + (i++) * m_tStep;
    if (t <= m_tStart) {
      t = m_tStart + FLT_EPSILON;
    }
    if (t >= m_tEnd) {
      t = m_tEnd - FLT_EPSILON;
      isLastSegment = true;
    }
    if (previousT == t) {
      // No need to draw segment. Happens when tStep << tStart .
      continue;
    }
    previousXY = xy;
    xy = m_curve.evaluate(t, m_context);

    // Draw a line with the pattern
    float patternMin = ((m_patternLowerBound ? m_patternLowerBound.evaluate(t, m_context) : xy).*ordinate)();
    float patternMax = ((m_patternUpperBound ? m_patternUpperBound.evaluate(t, m_context) : xy).*ordinate)();
    if (m_patternWithoutCurve) {
      if (std::isnan(patternMin)) {
        patternMin = -INFINITY;
      }
      if (std::isnan(patternMax)) {
        patternMax = INFINITY;
      }
    }
    if (!(std::isnan(patternMin) || std::isnan(patternMax)) && patternMin != patternMax && m_patternStart <= t && t < m_patternEnd) {
      m_pattern.drawInLine(plotView, ctx, rect, AbstractPlotView::OtherAxis(m_axis), (xy.*abscissa)(), patternMin, patternMax);
    }

    joinDots(plotView, ctx, rect, previousT, previousXY, t, xy, k_maxNumberOfIterations, m_discontinuity);
  } while (!isLastSegment);

  plotView->setDashed(false);
}

static bool pointInBoundingBox(float x1, float y1, float x2, float y2, float xC, float yC) {
  return ((x1 < xC && xC < x2) || (x2 < xC && xC < x1) || (x2 == xC && xC == x1))
      && ((y1 < yC && yC < y2) || (y2 < yC && yC < y1) || (y2 == yC && yC == y1));
}

void WithCurves::CurveDrawing::joinDots(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, float t1, Coordinate2D<float> xy1, float t2, Coordinate2D<float> xy2, int remainingIterations, DiscontinuityTest discontinuity) const {
  assert(plotView);

  bool isFirstDot = std::isnan(t1);
  bool isLeftDotValid = std::isfinite(xy1.x1()) && std::isfinite(xy1.x2());
  bool isRightDotValid = std::isfinite(xy2.x1()) && std::isfinite(xy2.x2());
  if (!(isLeftDotValid || isRightDotValid)) {
    return;
  }

  Coordinate2D<float> p1 = plotView->floatToPixel2D(xy1);
  Coordinate2D<float> p2 = plotView->floatToPixel2D(xy2);
  if (isRightDotValid) {
    if (isFirstDot || (!isLeftDotValid && remainingIterations <= 0) || (isLeftDotValid && plotView->pointsInSameStamp(p1, p2, m_thick))) {
      /* We need to be sure that the point is not an artifact caused by error
       * in float approximation. */
      Coordinate2D<float> p2Double = m_curveDouble ? plotView->floatToPixel2D(m_curveDouble(t2, m_curve.model(), m_context)) : p2;
      if (std::isnan(p2Double.x1()) || std::isnan(p2Double.x2())) {
        p2Double = p2;
      }
      plotView->stamp(ctx, rect, p2, m_color, m_thick);
      return;
    }
  }

  float t12 = 0.5f * (t1 + t2);
  Coordinate2D<float> xy12 = m_curve.evaluate(t12, m_context);

  bool discontinuous = discontinuity(t1, t2, m_curve.model(), m_context);
  if (discontinuous) {
    /* If the function is discontinuous, it can never join dots at abscissas of
     * discontinuity, and thus will always go to max recursion. To avoid this,
     * and enhance performances, we set the condition that if one of the two
     * dots left and right of the discontinuity is on the same pixel as the
     * middle dot, we are close enough of the discontinuity and we can stop
     * drawing more precisely. */
    Coordinate2D<float> p12 = plotView->floatToPixel2D(xy12);
    if (isRightDotValid && (plotView->pointsInSameStamp(p1, p12, m_thick) || plotView->pointsInSameStamp(p12, p2, m_thick))) {
      plotView->stamp(ctx, rect, p2, m_color, m_thick);
      return;
    }
  } else if (isLeftDotValid && isRightDotValid && (m_drawStraightLinesEarly || remainingIterations <= 0) && pointInBoundingBox(xy1.x1(), xy1.x2(), xy2.x1(), xy2.x2(), xy12.x1(), xy12.x2())) {
    /* As the middle dot is between the two dots, we assume that we
     * can draw a 'straight' line between the two */
    constexpr float dangerousSlope = 1e6f;
    if (m_curveDouble && std::fabs((p2.x2() - p1.x2()) / (p2.x1() - p1.x1())) > dangerousSlope) {
      /* We need to make sure we're not drawing a vertical asymptote because of
       * rounding errors. */
      Coordinate2D<double> xy1Double = m_curveDouble(t1, m_curve.model(), m_context);
      Coordinate2D<double> xy2Double = m_curveDouble(t2, m_curve.model(), m_context);
      Coordinate2D<double> xy12Double = m_curveDouble(t12, m_curve.model(), m_context);
      if (pointInBoundingBox(xy1Double.x1(), xy1Double.x2(), xy2Double.x1(), xy2Double.x2(), xy12Double.x1(), xy12Double.x2())) {
        plotView->straightJoinDots(ctx, rect, plotView->floatToPixel2D(xy1Double), plotView->floatToPixel2D(xy2Double), m_color, m_thick);
        return;
      }
    } else {
      plotView->straightJoinDots(ctx, rect, p1, p2, m_color, m_thick);
      return;
    }
  }

  if (remainingIterations > 0) {
    remainingIterations--;

    CurveViewRange * range = plotView->range();
    float xMin = range->xMin();
    float xMax = range->xMax();
    float yMin = range->yMin();
    float yMax = range->yMax();
    if ((xMax < xy1.x1() && xMax < xy2.x1()) || (xy1.x1() < xMin && xy2.x1() < xMin) || (yMax < xy1.x2() && yMax < xy2.x2()) || (xy1.x2() < yMin && xy2.x2() < yMin)) {
      /* Discard some recursion steps to save computation time on dots that are
       * likely not to be drawn. This makes it so some parametric functions
       * are drawn faster. Example: f(t) = [floor(t)*cos(t), floor(t)*sin(t)]
       * If t is in [0, 60pi], and you zoom in a lot, the curve used to take
       * too much time to draw outside of the screen.
       * It can alter precision with some functions though, especially when
       * zooming excessively (compared to plot range) on local minimums
       * For instance, plotting parametric function [t,|t-π|] with t in [0,3000],
       * x in [-1,20] and y in [-1,3] will show inaccuracies that would
       * otherwise have been visible at higher zoom only, with x in [2,4] and y
       * in [-0.2,0.2] in this case. */
      remainingIterations /= 2;
    }

    joinDots(plotView, ctx, rect, t1, xy1, t12, xy12, remainingIterations, discontinuous ? m_discontinuity : NoDiscontinuity);
    joinDots(plotView, ctx, rect, t12, xy12, t2, xy2, remainingIterations, discontinuous ? m_discontinuity : NoDiscontinuity);
  }
}

// WithCurves

void WithCurves::drawArcOfEllipse(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect, Coordinate2D<float> center, float width, float height, float angleStart, float angleEnd, KDColor color) const {
  assert(plotView);

  constexpr float segmentLength = 2.f;
  float radiusInPixel = std::max(width / plotView->pixelWidth(), height / plotView->pixelHeight());
  float angleStep = segmentLength / radiusInPixel;
  float parameters[] = { center.x1(), center.x2(), width, height };
  Curve2DEvaluation<float> arc = [](float t, void * model, void *) {
    float * parameters = reinterpret_cast<float *>(model);
    float x = parameters[0];
    float y = parameters[1];
    float a = parameters[2];
    float b = parameters[3];
    return Coordinate2D<float>(a * std::cos(t) + x, b * std::sin(t) + y);
  };
  CurveDrawing plot(Curve2D(arc, parameters), nullptr, angleStart, angleEnd, angleStep, color, false);
  plot.setPrecisionOptions(false, nullptr, NoDiscontinuity);
  plot.draw(plotView, ctx, rect);
}

// WithHistogram::HistogramDrawing

WithHistogram::HistogramDrawing::HistogramDrawing(Curve1D curve, void * model, void * context, HighlightTest highlightTest, double start, double barsWidth, bool displayBorder, bool fillBars, KDColor color, KDColor highlightColor, KDColor borderColor) :
  m_curve(curve),
  m_model(model),
  m_context(context),
  m_highlightTest(highlightTest),
  m_start(start),
  m_barsWidth(barsWidth),
  m_displayBorder(displayBorder),
  m_fillBars(fillBars),
  m_color(color),
  m_highlightColor(highlightColor),
  m_borderColor(borderColor)
{}

void WithHistogram::HistogramDrawing::draw(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  /* To values interval [a, b[ correspond a pixel
   * interval [A, B[. Tick for a is at pixel A and
   * tick for b is at pixel B.
   *
   *  ---------------------------------------
   * | A |   |   |   |   |   |   |   |   | B |
   *  ---------------------------------------
   *  <--------------------------------->
   *                bar width
   *
   * If we want to diplays borders:
   *  - left border is drawn at A
   *  - right border is drawn at B
   */
  double rectMin = plotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, rect.left());
  double rectMinBarIndex = std::floor((rectMin - m_start) / m_barsWidth);
  double rectMinBarStart = m_start + rectMinBarIndex * m_barsWidth;
  double rectMax = plotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, rect.right());
  double rectMaxBarIndex = std::floor((rectMax - m_start) / m_barsWidth);
  double rectMaxBarEnd = m_start + (rectMaxBarIndex + 1) * m_barsWidth;
  double step = std::max(plotView->pixelWidth(), static_cast<float>(m_barsWidth));
  KDCoordinate plotViewHeight = plotView->floatToKDCoordinatePixel(AbstractPlotView::Axis::Vertical, 0.f);
  KDCoordinate borderWidth = m_displayBorder ? k_borderWidth : 0;

  double xPrevious = NAN;
  for (double x = rectMinBarStart; x < rectMaxBarEnd; x += step) {
    if (x == xPrevious) {
      return;
    }
    xPrevious = x;

    // Step 1: Compute values
    double xCenter = m_fillBars ? x + 0.5f * m_barsWidth : x;
    double y = m_curve(xCenter, m_model, m_context);
    if (std::isnan(y) || y == 0.f) {
      continue;
    }
    assert(y > 0.f); /* TODO This method is not ready to display histogram with negative values. */

    // Step 2: Compute pixels
    // Step 2.1: Bar width
    KDCoordinate left = plotView->floatToKDCoordinatePixel(AbstractPlotView::Axis::Horizontal, x);
    KDCoordinate leftOfNextBar = plotView->floatToKDCoordinatePixel(AbstractPlotView::Axis::Horizontal, x + m_barsWidth);
    if (leftOfNextBar <= rect.left()) {
      continue;
    }
    KDCoordinate barWidth;
    /* If m_fillBar is true, we fill the space between one bar and the next
     * (e.g. histograms in the Statistics app). Otherwise, we only draw a thin
     * bar on lefmost pixels (e.g. histograms for discrete distributions). */
    if (m_fillBars) {
      barWidth = leftOfNextBar - left;
    } else {
      assert(borderWidth == 0);
      barWidth = k_hollowBarWidth;
    }

    // Step 2.2: Bar height
    KDCoordinate top = plotView->floatToKDCoordinatePixel(AbstractPlotView::Axis::Vertical, y);
    KDCoordinate barHeight = plotViewHeight - top;
    assert(barHeight >= 0);

    // Step 3: Draw
    KDColor color = m_highlightTest && m_highlightTest(xCenter, m_model, m_context) ? m_highlightColor : m_color;
    // Interior
    KDRect barRect(left + borderWidth, top + borderWidth, barWidth - borderWidth, barHeight - borderWidth);
    ctx->fillRect(barRect, color);
    if (borderWidth > 0 && barRect.width() > 0 && barRect.height() > 0) {
      // Left border
      ctx->fillRect(KDRect(left, top, borderWidth, barHeight), m_borderColor);
      // Top border
      ctx->fillRect(KDRect(left + borderWidth, top, barWidth, borderWidth), m_borderColor);
      // Right border
      ctx->fillRect(KDRect(leftOfNextBar, top, borderWidth, barHeight), m_borderColor);
    }
  }
}

}
}
