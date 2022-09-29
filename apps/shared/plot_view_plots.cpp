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
  KDCoordinate posC = plotView->floatToPixel(perpendicular, position);

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
    plotView->drawStraightSegment(ctx, rect, parallel, position, min, max, m_cBackground);
    return;
  }

  if (max < min) {
    std::swap(max, min);
  }
  KDCoordinate minC = std::round(plotView->floatToPixel(parallel, min));
  KDCoordinate maxC = std::round(plotView->floatToPixel(parallel, max));
  KDRect boundingRect = parallel == AbstractPlotView::Axis::Horizontal ? KDRect(minC, posC, maxC - minC, 1) : KDRect(posC, maxC, 1, minC - maxC);
  boundingRect = boundingRect.intersectedWith(rect);
  if (parallel == AbstractPlotView::Axis::Horizontal) {
    minC = boundingRect.left();
    maxC = boundingRect.right();
  } else {
    minC = boundingRect.top();
    maxC = boundingRect.bottom();
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

  for (int i = minC; i <= maxC; i++) {
    KDColor color = colors[i % k_size];
    if (color != k_transparent) {
      ctx->setPixel(parallel == AbstractPlotView::Axis::Horizontal ? KDPoint(i, posC) : KDPoint(posC, i), color);
    }
  }
}

// WithCurves::CurveDrawing

WithCurves::CurveDrawing::CurveDrawing(Curve2D<float> curve, void * model, void * context, float tStart, float tEnd, float tStep, KDColor color, bool thick, bool dashed) :
  m_pattern(),
  m_model(model),
  m_context(context),
  m_curve(curve),
  m_patternLowerBound(nullptr),
  m_patternUpperBound(nullptr),
  m_curveDouble(nullptr),
  m_discontinuity(NoDiscontinuity),
  m_tStart(tStart),
  m_tEnd(tEnd),
  m_tStep(tStep),
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
}

void WithCurves::CurveDrawing::setPatternOptions(Pattern pattern, float patternStart, float patternEnd, Curve2D<float> patternLowerBound, Curve2D<float> patternUpperBound, bool patternWithoutCurve, AbstractPlotView::Axis axis) {
  m_pattern = pattern;
  m_patternStart = patternStart;
  m_patternEnd = patternEnd;
  m_patternLowerBound = patternLowerBound;
  m_patternUpperBound = patternUpperBound;
  m_patternWithoutCurve = patternWithoutCurve;
  m_axis = axis;
}

void WithCurves::CurveDrawing::setPrecisionOptions(bool drawStraightLinesEarly, Curve2D<double> curveDouble, DiscontinuityTest discontinuity) {
  m_drawStraightLinesEarly = drawStraightLinesEarly;
  m_curveDouble = curveDouble;
  m_discontinuity = discontinuity;
}

void WithCurves::CurveDrawing::draw(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  assert(plotView);
  assert(m_tStart <= m_tEnd);

  plotView->setDashed(m_dashed);

  /* ContinuousFunction caching relies on a consistent tStart and tStep. These
   * values shouldn't be altered here. */
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
    xy = m_curve(t, m_model, m_context);

    // Draw a line with the pattern
    float patternMin = ((m_patternLowerBound ? m_patternLowerBound(t, m_model, m_context) : xy).*ordinate)();
    float patternMax = ((m_patternUpperBound ? m_patternUpperBound(t, m_model, m_context) : xy).*ordinate)();
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
      Coordinate2D<float> p2Double = m_curveDouble ? plotView->floatToPixel2D(m_curveDouble(t2, m_model, m_context)) : p2;
      if (std::isnan(p2Double.x1()) || std::isnan(p2Double.x2())) {
        p2Double = p2;
      }
      plotView->stamp(ctx, rect, p2, m_color, m_thick);
      return;
    }
  }

  float t12 = 0.5f * (t1 + t2);
  Coordinate2D<float> xy12 = m_curve(t12, m_model, m_context);

  bool discontinuous = discontinuity(t1, t2, m_model, m_context);
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
      Coordinate2D<double> xy1Double = m_curveDouble(t1, m_model, m_context);
      Coordinate2D<double> xy2Double = m_curveDouble(t2, m_model, m_context);
      Coordinate2D<double> xy12Double = m_curveDouble(t12, m_model, m_context);
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
  float radiusInPixel = std::max(plotView->pixelWidth() * width, plotView->pixelHeight() * height);
  float angleStep = segmentLength / radiusInPixel;
  float parameters[] = { center.x1(), center.x2(), width, height };
  Curve2D<float> arc = [](float t, void * model, void *) {
    float * parameters = reinterpret_cast<float *>(model);
    float x = parameters[0];
    float y = parameters[1];
    float a = parameters[2];
    float b = parameters[3];
    return Coordinate2D<float>(a * std::cos(t) + x, b * std::sin(t) + y);
  };
  CurveDrawing plot(arc, parameters, nullptr, angleStart, angleEnd, angleStep, color, false);
  plot.setPrecisionOptions(false, nullptr, NoDiscontinuity);
  plot.draw(plotView, ctx, rect);
}

// WithHistogram::HistogramDrawing

WithHistogram::HistogramDrawing::HistogramDrawing(Curve1D curve, void * model, void * context, float start, float barsWidth, KDColor color, bool fillBars) :
  m_curve(curve),
  m_model(model),
  m_context(context),
  m_highlighted(nullptr),
  m_start(start),
  m_width(barsWidth),
  m_borderWidth(0.f),
  m_color(color),
  m_fillBars(fillBars)
{}

void WithHistogram::HistogramDrawing::setBorderOptions(KDCoordinate width, KDColor color) {
  m_borderWidth = width;
  m_borderColor = color;
}

void WithHistogram::HistogramDrawing::setHighlightOptions(HighlightTest highlighted, KDColor color) {
  m_highlighted = highlighted;
  m_highlightColor = color;
}

void WithHistogram::HistogramDrawing::draw(const AbstractPlotView * plotView, KDContext * ctx, KDRect rect) const {
  float rectMin = plotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, rect.left());
  float rectMax = plotView->pixelToFloat(AbstractPlotView::Axis::Horizontal, rect.right());
  float step = std::max(plotView->pixelWidth(), m_width);
  KDCoordinate axisPixel = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Vertical, 0.f));

  float xPrevious = NAN;
  for (float x = m_start; x < rectMax; x += step) {
    if (x == xPrevious) {
      return;
    }
    xPrevious = x;

    float xCenter = m_fillBars ? x + 0.5f * m_width : x;
    float y = m_curve(xCenter, m_model, m_context);
    if (std::isnan(y) || y == 0.f) {
      continue;
    }
    /* TODO This method is not ready to display histogram with negative values. */
    assert(y > 0.f);

    Coordinate2D<float> pxy = plotView->floatToPixel2D(Coordinate2D<float>(x, y));
    KDCoordinate pxLeft = std::round(pxy.x1());
    KDCoordinate pxRight = std::round(plotView->floatToPixel(AbstractPlotView::Axis::Horizontal, x + m_width));
    if (pxRight + m_borderWidth < rectMin) {
      continue;
    }
    KDCoordinate py = std::round(pxy.x2());
    KDCoordinate barHeight = axisPixel - py;
    assert(barHeight >= 0);
    KDCoordinate barWidth;
    if (m_fillBars) {
      barWidth = pxRight - pxLeft;
    } else {
      assert(m_borderWidth == 0);
      barWidth = k_hollowBarWidth;
    }
    KDRect barRect(pxLeft + m_borderWidth, py + m_borderWidth, barWidth, barHeight - m_borderWidth);
    KDColor color = m_highlighted && m_highlighted(xCenter, m_model, m_context) ? m_highlightColor : m_color;
    ctx->fillRect(barRect, color);

    if (m_borderWidth > 0 && barRect.width() > 0 && barRect.height() > 0) {
      ctx->fillRect(KDRect(pxLeft, py, m_borderWidth, barHeight), m_borderColor);
      ctx->fillRect(KDRect(pxLeft + m_borderWidth, py, barWidth, m_borderWidth), m_borderColor);
      ctx->fillRect(KDRect(pxRight, py, m_borderWidth, barHeight), m_borderColor);
    }
  }
}

}
}
