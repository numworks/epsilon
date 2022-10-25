#include "graph_view.h"
#include "../app.h"
#include <poincare/trigonometry.h>

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Graph {

GraphView::GraphView(InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, Shared::BannerView * bannerView, CursorView * cursorView) :
  FunctionGraphView(graphRange, cursor, bannerView, cursorView),
  m_tangent(false)
{}

void GraphView::reload(bool resetInterrupted, bool force) {
  if (m_tangent) {
    assert(m_banner);
    KDRect dirtyZone(KDRect(0, 0, bounds().width(), bounds().height() - m_banner->bounds().height()));
    markRectAsDirty(dirtyZone);
  }
  return FunctionGraphView::reload(resetInterrupted, force);
}

void GraphView::setFocus(bool focus) {
  if (focus != hasFocus()) {
    /* Points of interest change visibility when the focus changes. */
    markRectAsDirty(bounds());
  }
  FunctionGraphView::setFocus(focus);
}

int GraphView::numberOfDrawnRecords() const {
  return App::app()->functionStore()->numberOfActiveFunctions();
}

void GraphView::drawRecord(int i, KDContext * ctx, KDRect rect) const {
  if (i == 0) {
    m_areaIndex = 0;
  }

  ContinuousFunctionStore * functionStore = App::app()->functionStore();
  Ion::Storage::Record record = functionStore->activeRecordAtIndex(i);
  ExpiringPointer<ContinuousFunction> f = functionStore->modelForRecord(record);

  Expression e = f->expressionReduced(context());
  ContinuousFunctionProperties::AreaType area = f->properties().areaType();
  assert(f->numberOfSubCurves() <= 2);
  bool hasTwoCurves = (f->numberOfSubCurves() == 2);
  if (area == ContinuousFunctionProperties::AreaType::None) {
    if (e.type() == ExpressionNode::Type::Dependency) {
      e = e.childAtIndex(0);
    }
    bool isUndefined = e.isUndefined();
    if (!isUndefined && (f->properties().isParametric() || hasTwoCurves)) {
      assert(e.type() == ExpressionNode::Type::Matrix);
      assert(static_cast<Matrix&>(e).numberOfRows() == 2);
      assert(static_cast<Matrix&>(e).numberOfColumns() == 1);
      isUndefined = e.childAtIndex(0).isUndefined() && e.childAtIndex(1).isUndefined();
    }
    if (isUndefined) {
      // There is no need to plot anything.
      return;
    }
  }

  ContinuousFunctionCache * cch = functionStore->cacheAtIndex(i);
  float tmin = f->tMin();
  float tmax = f->tMax();
  Axis axis = f->isAlongY() ? Axis::Vertical : Axis::Horizontal;
  KDCoordinate rectMin = axis == Axis::Horizontal ? rect.left() - k_externRectMargin : rect.bottom() + k_externRectMargin;
  KDCoordinate rectMax = axis == Axis::Horizontal ? rect.right() + k_externRectMargin : rect.top() - k_externRectMargin;
  float tCacheMin, tCacheStep;
  float tStepNonCartesian = NAN;
  if (f->properties().isCartesian()) {
    float rectLimit = pixelToFloat(axis, rectMin);
    /* Here, tCacheMin can depend on rect (and change as the user move)
     * because cache can be panned for cartesian curves, instead of being
     * entirely invalidated. */
    tCacheMin = std::isnan(rectLimit) ? tmin : std::max(tmin, rectLimit);
    tmax = std::min(pixelToFloat(axis, rectMax), tmax);
    tCacheStep = axis == Axis::Horizontal ? pixelWidth() : pixelHeight();
  } else {
    tCacheMin = tmin;
    // Compute tCacheStep and tStepNonCartesian
    ContinuousFunctionCache::ComputeNonCartesianSteps(&tStepNonCartesian, &tCacheStep, tmax, tmin);
  }
  ContinuousFunctionCache::PrepareForCaching(f.operator->(), cch, tCacheMin, tCacheStep);

  /* Check now if e can be discontinuous: In case e does not involves
   * discontinuous functions, this avoids recomputing potential
   * discontinuity at each dot of the curve. */
  DiscontinuityTest discontinuityEvaluation = e.involvesDiscontinuousFunction(context()) ? FunctionIsDiscontinuousBetweenFloatValues : NoDiscontinuity;

  if (f->properties().isParametric()) {
    drawParametric(ctx, rect, f.operator->(), tCacheMin, tmax, tStepNonCartesian, discontinuityEvaluation);
    return;
  }
  if (f->properties().isPolar()) {
    drawPolar(ctx, rect, f.operator->(), tCacheMin, tmax, tStepNonCartesian, discontinuityEvaluation);
    return;
  }
  assert(f->properties().isCartesian());
  drawCartesian(ctx, rect, f.operator->(), record, tCacheMin, tmax, tCacheStep, discontinuityEvaluation, axis, rectMin, rectMax);
}

void GraphView::drawForeground(KDContext * ctx, KDRect rect) const {
  // Draw points of interest above all the curves.
  if (!hasFocus()) {
    return;
  }
  ContinuousFunctionStore * functionStore = App::app()->functionStore();
  ExpiringPointer<ContinuousFunction> f = functionStore->modelForRecord(m_selectedRecord);
  if (f->plotType() != ContinuousFunction::PlotType::Cartesian) {
    return;
  }
  Axis axis = f->isAlongY() ? Axis::Vertical : Axis::Horizontal;

  PointsOfInterestCache * pointsOfInterest = App::app()->graphController()->pointsOfInterest();
  for (const PointOfInterest<double> & p : pointsOfInterest->filter(m_interest)) {
    Coordinate2D<float> xy = axis == Axis::Horizontal ? static_cast<Coordinate2D<float>>(p.xy()) : Coordinate2D<float>(p.y(), p.x());
    drawDot(ctx, rect, Dots::Size::Tiny, xy, Palette::GrayDarkest);
  }
}

void GraphView::tidyModel(int i) const {
  ContinuousFunctionStore * store = App::app()->functionStore();
  store->modelForRecord(store->activeRecordAtIndex(i))->tidyDownstreamPoolFrom();
}

template<typename T>
static Coordinate2D<T> evaluateXY(T t, void * model, void * context) { return reinterpret_cast<ContinuousFunction *>(model)->evaluateXYAtParameter(t, reinterpret_cast<Context *>(context), 0); }
template<typename T>
static Coordinate2D<T> evaluateXYSecondCurve(T t, void * model, void * context) { return reinterpret_cast<ContinuousFunction *>(model)->evaluateXYAtParameter(t, reinterpret_cast<Context *>(context), 1); }

static Coordinate2D<float> evaluateInfinity(float t, void *, void *) { return Coordinate2D<float>(INFINITY, INFINITY); }
static Coordinate2D<float> evaluateMinusInfinity(float t, void *, void *) { return Coordinate2D<float>(-INFINITY, -INFINITY); }
static Coordinate2D<float> evaluateZero(float t, void *, void *) { return Coordinate2D<float>(t, 0.f); }

bool GraphView::FunctionIsDiscontinuousBetweenFloatValues(float x1, float x2, void * model, void * context) {
  return static_cast<ContinuousFunction *>(model)->isDiscontinuousBetweenFloatValues(x1, x2, static_cast<Poincare::Context *>(context));
}

void GraphView::drawCartesian(KDContext * ctx, KDRect rect, ContinuousFunction * f, Ion::Storage::Record record, float tStart, float tEnd, float tStep, DiscontinuityTest discontinuity, Axis axis, KDCoordinate rectMin, KDCoordinate rectMax) const {
  ContinuousFunctionProperties::AreaType area = f->properties().areaType();
  bool hasTwoCurves = (f->numberOfSubCurves() == 2);

  // - Define the bounds of the colored area
  bool patternWithoutCurve = false;
  float patternStart = tStart, patternEnd = tEnd;
  Curve2D patternLower, patternUpper, patternLower2;
  Pattern pattern(m_areaIndex, f->color());

  switch (area) {
  case ContinuousFunctionProperties::AreaType::Outside:
    /* This relies on the fact that the second curve will be below the first. */
    (hasTwoCurves ? patternLower2 : patternLower) = Curve2D(evaluateMinusInfinity);
    patternUpper = Curve2D(evaluateInfinity);
    patternWithoutCurve = true;
    break;
  case ContinuousFunctionProperties::AreaType::Above:
    patternUpper = Curve2D(evaluateInfinity);
    break;
  case ContinuousFunctionProperties::AreaType::Below:
    (hasTwoCurves ? patternLower2 : patternLower) = Curve2D(evaluateMinusInfinity);
    break;
  case ContinuousFunctionProperties::AreaType::Inside:
    /* The function might not have two curves if the area is empty
     * (e.g. y^2<0). */
    if (hasTwoCurves) {
      patternLower = Curve2D(evaluateXYSecondCurve<float>, f);
    }
    break;
  default:
    assert(area == ContinuousFunctionProperties::AreaType::None);
    bool isIntegral = record == m_selectedRecord && std::isfinite(m_highlightedStart) && std::isfinite(m_highlightedEnd);
    if (isIntegral) {
      assert(!hasTwoCurves);
      if (m_secondSelectedRecord.isNull()) {
        patternLower = Curve2D(evaluateZero);
      } else {
        ContinuousFunction * otherModel = App::app()->functionStore()->modelForRecord(m_secondSelectedRecord).operator->();
        patternLower = Curve2D(evaluateXY, otherModel);
        pattern = Pattern(m_areaIndex, KDColor::HSVBlend(f->color(), otherModel->color()));
      }
      patternStart = m_highlightedStart;
      patternEnd = m_highlightedEnd;
    }
  }
  if (patternLower || patternUpper || patternLower2) {
    m_areaIndex = (m_areaIndex + 1) % Pattern::k_numberOfSections;
  }

  // - Draw first curve
  CurveDrawing firstCurve(Curve2D(evaluateXY<float>, f), context(), tStart, tEnd, tStep, f->color(), true, f->properties().plotIsDotted());
  firstCurve.setPrecisionOptions(true, evaluateXY<double>, discontinuity);
  firstCurve.setPatternOptions(pattern, patternStart, patternEnd, patternLower, patternUpper, patternWithoutCurve, axis);
  firstCurve.draw(this, ctx, rect);

  // - Draw second curve
  if (hasTwoCurves) {
    CurveDrawing secondCurve(Curve2D(evaluateXYSecondCurve<float>, f), context(), tStart, tEnd, tStep, f->color(), true, f->properties().plotIsDotted());
    secondCurve.setPrecisionOptions(true, evaluateXYSecondCurve<double>, discontinuity);
    secondCurve.setPatternOptions(pattern, patternStart, patternEnd, patternLower2, Curve2D(), patternWithoutCurve, axis);
    secondCurve.draw(this, ctx, rect);
  }

  // - Draw tangent
  if (m_tangent && m_selectedRecord == record) {
    assert(f->canDisplayDerivative());
    /* TODO : We could handle tangent on second curve here by finding out
     * which of the two curves is selected. */
    float tangentParameterA = f->approximateDerivative(m_cursor->x(), context(), 0);
    float tangentParameterB = -tangentParameterA * m_cursor->x() + f->evaluateXYAtParameter(m_cursor->x(), context(), 0).x2();
    /* To represent the tangent, we draw segment from and to abscissas at the
     * extremities of the drawn rect. */
    float minAbscissa = pixelToFloat(axis, rectMin);
    Coordinate2D<float> p1(minAbscissa, tangentParameterA * minAbscissa + tangentParameterB);
    float maxAbscissa = pixelToFloat(axis, rectMax);
    Coordinate2D<float> p2(maxAbscissa, tangentParameterA * maxAbscissa + tangentParameterB);
    drawSegment(ctx, rect, p1, p2, Palette::GrayVeryDark, false);
  }
}

static float polarThetaFromCoordinates(float x, float y, Preferences::AngleUnit angleUnit) {
  // Return θ, between -π and π in given angleUnit for a (x,y) position.
  return Trigonometry::ConvertRadianToAngleUnit<float>(std::arg(std::complex<float>(x,y)), angleUnit).real();
}

void GraphView::drawPolar(KDContext * ctx, KDRect rect, ContinuousFunction * f, float tStart, float tEnd, float tStep, DiscontinuityTest discontinuity) const {
  // Compute rect limits
  float rectLeft = pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectRight = pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  float rectUp = pixelToFloat(Axis::Vertical, rect.top() - k_externRectMargin);
  float rectDown = pixelToFloat(Axis::Vertical, rect.bottom() + k_externRectMargin);

  const Preferences::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  const float piInAngleUnit = Trigonometry::PiInAngleUnit(angleUnit);
  /* Cancel optimization if :
   * - One of rect limits is nan.
   * - Step is too large, see cache optimization comments
   *   ("To optimize cache..."). */
  bool cancelOptimization = std::isnan(rectLeft + rectRight + rectUp + rectDown) || tStep >= piInAngleUnit;

  bool rectOverlapsNegativeAbscissaAxis = false;
  if (cancelOptimization || (rectUp > 0.0f && rectDown < 0.0f && rectLeft < 0.0f)) {
    if (cancelOptimization || rectRight > 0.0f) {
      // Origin is inside rect, tStart and tEnd cannot be optimized
      return drawParametric(ctx, rect, f, tStart, tEnd, tStep, discontinuity);
    }
    // Rect view overlaps the abscissa, on the left of the origin.
    rectOverlapsNegativeAbscissaAxis = true;
  }

  float tMin, tMax;
  /* Compute angular coordinate of each corners of rect.
   * t3 --- t2
   *  |      |
   * t4 --- t1 */
  float t1 = polarThetaFromCoordinates(rectRight, rectDown, angleUnit);
  float t2 = polarThetaFromCoordinates(rectRight, rectUp, angleUnit);
  if (!rectOverlapsNegativeAbscissaAxis) {
    float t3 = polarThetaFromCoordinates(rectLeft, rectUp, angleUnit);
    float t4 = polarThetaFromCoordinates(rectLeft, rectDown, angleUnit);
    /* The area between tMin and tMax (modulo π) is the only area where
     * something needs to be plotted. */
    tMin = std::min({t1, t2, t3, t4});
    tMax = std::max({t1, t2, t3, t4});
  } else {
    /* polarThetaFromCoordinates yields coordinates between -π and π. When rect
     * is overlapping the negative abscissa (at this point, the origin cannot be
     * inside rect), t1 and t4 have a negative angle whereas t2 and t3 have a
     * positive angle. We ensure here that tMin is t2 (modulo 2π), tMax is t1,
     * and that tMax-tMin is minimal and positive. */
    tMin = t2 - 2 * piInAngleUnit;
    tMax = t1;
  }

  // Add a thousandth of π as a margin to avoid visible approximation errors.
  tMax += piInAngleUnit / 1000.0f;
  tMin -= piInAngleUnit / 1000.0f;

  /* To optimize cache hits, the area actually drawn will be extended to nearest
   * cached θ. tStep being a multiple of cache steps (see
   * ComputeNonCartesianSteps), we extend segments on both ends to the closest
   * θ = tStart + tStep * i
   * If the drawn segment is extended too much, it might overlap with the next
   * extended segment.
   * For example, with * the segments that must be drawn and piInAngleUnit=7 :
   *                 tStart                                            tEnd
   *              kπ   | (k+1)π  (k+2)π  (k+3)π  (k+4)π  (k+5)π  (k+6)π  |(k+7)π
   *               |-------|-------|-------|-------|-------|-------|-------|--
   * tMax-tMin=3 : |---***-|---***-|---***-|---***-|---***-|---***-|---***-|--
   * A - tStep=3 : |---***-|---***-|---***-|---***-|---***-|---***-|---***-|--
   *               |___^^^_|__     | ^^^^^^|___   _|__^^^^^|^      |___^^^_|__
   *               |       |  ^^^^^|^      |   ^^^ |       | ^^^^^^|       |
   *
   * B - tStep=6 : |---***-|---***-|---***-|---***-|---***-|---***-|---***-|--
   *               |___^^^^|^^     | ^^^^^^|      ^|^^^^^^^|^^^^   |   ^^^^|^^
   *               |       |  ^^^^^|^      |^^^^^^ |     ^^|^^^^^^^|^^^    |
   * In situation A, Step are small enough, not all segments must be drawn.
   * In situation B, The entire range should be drawn, and two extended segments
   * overlap (at tStart+5*tStep). Optimization is useless.
   * If tStep < piInAngleUnit - (tMax - tMin), situation B cannot happen. */
  if (tStep >= piInAngleUnit - tMax + tMin) {
    return drawParametric(ctx, rect, f, tStart, tEnd, tStep, discontinuity);
  }

  /* The number of segments to draw can be reduced by drawing curve on intervals
   * where (tMin%π, tMax%π) intersects (tStart, tEnd).For instance :
   * if tStart=-π, tEnd=3π, tMin=π/4 and tMax=π/3, a curve is drawn between :
   * - [ π/4, π/3 ], [ 2π + π/4, 2π + π/3 ]
   * - [ -π + π/4, -π + π/3 ], [ π + π/4, π + π/3 ] in case f(θ) is negative */

  // 1 - Set offset so that tStart <= tMax+thetaOffset < piInAngleUnit+tStart
  float thetaOffset = std::ceil((tStart - tMax)/piInAngleUnit) * piInAngleUnit;

  // 2 - Increase offset until tMin + thetaOffset > tEnd
  float tCache2 = tStart;
  while (tMin + thetaOffset <= tEnd) {
    float tS = std::max(tMin + thetaOffset, tStart);
    float tE = std::min(tMax + thetaOffset, tEnd);
    // Draw curve if there is an intersection
    if (tS <= tE) {
      /* To maximize cache hits, we floor (and ceil) tS (and tE) to the closest
       * cached value. Step is small enough so that the extra drawn curve cannot
       * overlap as tMax + tStep < piInAngleUnit + tMin) */
      int i = std::floor((tS - tStart) / tStep);
      assert(tStart + tStep * i >= tCache2);
      float tCache1 = tStart + tStep * i;

      int j = std::ceil((tE - tStart) / tStep);
      tCache2 = std::min(tStart + tStep * j, tEnd);

      assert(tCache1 <= tCache2);
      drawParametric(ctx, rect, f, tCache1, tCache2, tStep, discontinuity);
    }
    thetaOffset += piInAngleUnit;
  }
}

void GraphView::drawParametric(KDContext * ctx, KDRect rect, ContinuousFunction * f, float tStart, float tEnd, float tStep, DiscontinuityTest discontinuity) const {
  CurveDrawing plot(Curve2D(evaluateXY<float>, f), context(), tStart, tEnd, tStep, f->color());
  plot.setPrecisionOptions(false, nullptr, discontinuity);
  plot.draw(this, ctx, rect);
}

}
