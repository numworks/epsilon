#include "graph_view.h"

#include <poincare/trigonometry.h>

#include "../app.h"

using namespace Escher;
using namespace Poincare;
using namespace Shared;

namespace Graph {

GraphView::GraphView(InteractiveCurveViewRange *graphRange,
                     CurveViewCursor *cursor, Shared::BannerView *bannerView,
                     MemoizedCursorView *cursorView)
    : FunctionGraphView(graphRange, cursor, bannerView, cursorView),
      m_interestView(this),
      m_areaIndex(0),
      m_nextPointOfInterestIndex(0),
      m_interest(Solver<double>::Interest::None),
      m_computePointsOfInterest(false),
      m_tangentDisplay(false) {}

void GraphView::reload(bool resetInterrupted, bool force) {
  if (m_tangentDisplay) {
    markRectAsDirty(boundsWithoutBanner());
  }
  return FunctionGraphView::reload(resetInterrupted, force);
}

void GraphView::drawRect(KDContext *ctx, KDRect rect) const {
  if (rect.intersectedWith(boundsWithoutBanner()) == boundsWithoutBanner()) {
    // If the whole curve is redrawn, all points of interest need a redraw
    m_nextPointOfInterestIndex = 0;
  }
  FunctionGraphView::drawRect(ctx, rect);
}

int GraphView::selectedRecordIndex() const {
  return functionStore()->indexOfRecordAmongActiveRecords(m_selectedRecord);
}

void GraphView::setFocus(bool focus) {
  if (focus != hasFocus()) {
    /* Points of interest change visibility when the focus changes. */
    markWholeFrameAsDirty();
  }
  FunctionGraphView::setFocus(focus);
}

int GraphView::numberOfDrawnRecords() const {
  return functionStore()->numberOfActiveFunctions();
}

void GraphView::drawRecord(Ion::Storage::Record record, int index,
                           KDContext *ctx, KDRect rect,
                           bool firstDrawnRecord) const {
  if (firstDrawnRecord) {
    m_areaIndex = 0;
  }

  ExpiringPointer<ContinuousFunction> f =
      functionStore()->modelForRecord(record);

  Expression e = f->expressionApproximated(context());
  ContinuousFunctionProperties::AreaType area = f->properties().areaType();
  assert(f->numberOfSubCurves() <= 2);
  if (area == ContinuousFunctionProperties::AreaType::None) {
    if (e.type() == ExpressionNode::Type::Dependency) {
      e = e.childAtIndex(0);
    }
    bool isUndefined = e.isUndefined();
    if (!isUndefined && e.type() == ExpressionNode::Type::Point) {
      assert(f->properties().isParametric() || f->properties().isScatterPlot());
      assert(e.numberOfChildren() == 2);
      isUndefined =
          e.childAtIndex(0).isUndefined() || e.childAtIndex(1).isUndefined();
    }
    if (!isUndefined && f->numberOfSubCurves() == 2) {
      assert(e.type() == ExpressionNode::Type::List);
      assert(e.numberOfChildren() == 2);
      isUndefined =
          e.childAtIndex(0).isUndefined() && e.childAtIndex(1).isUndefined();
    }
    if (isUndefined) {
      // There is no need to plot anything.
      return;
    }
  }

  ContinuousFunctionCache *cch = functionStore()->cacheAtIndex(index);
  float tmin = f->tMin();
  float tmax = f->tMax();
  Axis axis = f->isAlongY() ? Axis::Vertical : Axis::Horizontal;
  KDCoordinate rectMin = axis == Axis::Horizontal
                             ? rect.left() - k_externRectMargin
                             : rect.bottom() + k_externRectMargin;
  KDCoordinate rectMax = axis == Axis::Horizontal
                             ? rect.right() + k_externRectMargin
                             : rect.top() - k_externRectMargin;
  float tCacheMin, tStep, tCacheStep;
  if (f->properties().isCartesian()) {
    float rectLimit = pixelToFloat(axis, rectMin);
    /* Here, tCacheMin can depend on rect (and change as the user move)
     * because cache can be panned for cartesian curves, instead of being
     * entirely invalidated. */
    tCacheMin = std::isnan(rectLimit) ? tmin : std::max(tmin, rectLimit);
    tmax = std::min(pixelToFloat(axis, rectMax), tmax);
    tStep = axis == Axis::Horizontal ? pixelWidth() : pixelHeight();
    tCacheStep = tStep / 2.;
  } else {
    tCacheMin = tmin;
    // Compute tCacheStep and tStepNonCartesian
    ContinuousFunctionCache::ComputeNonCartesianSteps(&tStep, &tCacheStep, tmax,
                                                      tmin);
  }
  ContinuousFunctionCache::PrepareForCaching(f.operator->(), cch, tCacheMin,
                                             tCacheStep);

  /* Check now if e can be discontinuous: In case e does not involves
   * discontinuous functions, this avoids recomputing potential
   * discontinuity at each dot of the curve. */
  DiscontinuityTest discontinuityEvaluation =
      e.involvesDiscontinuousFunction(context())
          ? FunctionIsDiscontinuousBetweenFloatValues
          : NoDiscontinuity;

  if (f->properties().isCartesian()) {
    drawCartesian(ctx, rect, f.operator->(), record, tCacheMin, tmax, tStep,
                  discontinuityEvaluation, axis);

  } else if (f->properties().isPolar()) {
    drawPolar(ctx, rect, f.operator->(), tCacheMin, tmax, tStep,
              discontinuityEvaluation);

  } else if (f->properties().isScatterPlot()) {
    drawScatterPlot(ctx, rect, f.operator->());
  } else {
    assert(f->properties().isParametric() || f->properties().isInversePolar());
    drawFunction(ctx, rect, f.operator->(), tCacheMin, tmax, tStep,
                 discontinuityEvaluation);
  }

  // Draw tangent
  drawTangent(ctx, rect, record);
}

void GraphView::tidyModel(int i, PoolObject *treePoolCursor) const {
  functionStore()
      ->modelForRecord(functionStore()->activeRecordAtIndex(i))
      ->tidyDownstreamPoolFrom(treePoolCursor);
}

ContinuousFunctionStore *GraphView::functionStore() const {
  return App::app()->functionStore();
}

template <typename T>
static Coordinate2D<T> evaluateXY(T t, void *model, void *context) {
  return reinterpret_cast<ContinuousFunction *>(model)->evaluateXYAtParameter(
      t, reinterpret_cast<Context *>(context), 0);
}
template <typename T>
static Coordinate2D<T> evaluateXYSecondCurve(T t, void *model, void *context) {
  return reinterpret_cast<ContinuousFunction *>(model)->evaluateXYAtParameter(
      t, reinterpret_cast<Context *>(context), 1);
}
template <typename T>
static Coordinate2D<T> evaluateXYFirstDerivative(T t, void *model,
                                                 void *context) {
  return reinterpret_cast<ContinuousFunction *>(model)
      ->evaluateXYDerivativeAtParameter(t, reinterpret_cast<Context *>(context),
                                        1);
}
template <typename T>
static Coordinate2D<T> evaluateXYSecondDerivative(T t, void *model,
                                                  void *context) {
  return reinterpret_cast<ContinuousFunction *>(model)
      ->evaluateXYDerivativeAtParameter(t, reinterpret_cast<Context *>(context),
                                        2);
}

static Coordinate2D<float> evaluateInfinity(float t, void *, void *) {
  return Coordinate2D<float>(INFINITY, INFINITY);
}
static Coordinate2D<float> evaluateMinusInfinity(float t, void *, void *) {
  return Coordinate2D<float>(-INFINITY, -INFINITY);
}
static Coordinate2D<float> evaluateZero(float t, void *, void *) {
  return Coordinate2D<float>(t, 0.f);
}

bool GraphView::FunctionIsDiscontinuousBetweenFloatValues(float x1, float x2,
                                                          void *model,
                                                          void *context) {
  return static_cast<ContinuousFunction *>(model)
      ->isDiscontinuousBetweenFloatValues(
          x1, x2, static_cast<Poincare::Context *>(context));
}

template <typename T>
GraphView::Curve2DEvaluation<T> GraphView::subCurveEvaluation(
    ContinuousFunction *f, int subCurveIndex) const {
  if (subCurveIndex == 0) {
    return evaluateXY<T>;
  }
  if (f->numberOfSubCurves() > 1) {
    assert(f->numberOfSubCurves() == 2);
    assert(subCurveIndex == 1);
    return evaluateXYSecondCurve<T>;
  }
  assert(f->numberOfSubCurves() == 1);
  assert(f->numberOfSubCurves(true) > 1);
  int derivationOrder = f->derivationOrderFromSubCurveIndex(subCurveIndex);
  assert(derivationOrder == 1 || derivationOrder == 2);
  return derivationOrder == 1 ? evaluateXYFirstDerivative<T>
                              : evaluateXYSecondDerivative<T>;
}

void GraphView::drawCartesian(KDContext *ctx, KDRect rect,
                              ContinuousFunction *f,
                              Ion::Storage::Record record, float tStart,
                              float tEnd, float tStep,
                              DiscontinuityTest discontinuity,
                              Axis axis) const {
  assert(f->properties().isCartesian());
  ContinuousFunctionProperties::AreaType area = f->properties().areaType();
  bool hasTwoCurves = (f->numberOfSubCurves() == 2);

  // - Define the bounds of the colored area
  bool patternWithoutCurve = false;
  float patternStart = tStart, patternEnd = tEnd;
  Curve2D patternLower, patternUpper, patternLower2;
  Pattern pattern(m_areaIndex, f->color());

  switch (area) {
    case ContinuousFunctionProperties::AreaType::Outside:
      /* This relies on the fact that the second curve will be below the first.
       */
      (hasTwoCurves ? patternLower2 : patternLower) =
          Curve2D(evaluateMinusInfinity);
      patternUpper = Curve2D(evaluateInfinity);
      patternWithoutCurve = true;
      break;
    case ContinuousFunctionProperties::AreaType::Above:
      patternUpper = Curve2D(evaluateInfinity);
      break;
    case ContinuousFunctionProperties::AreaType::Below:
      (hasTwoCurves ? patternLower2 : patternLower) =
          Curve2D(evaluateMinusInfinity);
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
      bool isIntegral = record == m_selectedRecord &&
                        std::isfinite(m_highlightedStart) &&
                        std::isfinite(m_highlightedEnd);
      if (isIntegral) {
        assert(!hasTwoCurves);
        if (m_secondSelectedRecord.isNull()) {
          patternLower = Curve2D(evaluateZero);
        } else {
          ContinuousFunction *otherModel =
              functionStore()
                  ->modelForRecord(m_secondSelectedRecord)
                  .
                  operator->();
          assert(f->canComputeArea());
          assert(otherModel->canComputeArea());
          patternLower = Curve2D(evaluateXY, otherModel);
          pattern = Pattern(m_areaIndex,
                            KDColor::HSVBlend(f->color(), otherModel->color()));
        }
        patternStart = m_highlightedStart;
        patternEnd = m_highlightedEnd;
      }
  }
  if (patternLower || patternUpper || patternLower2) {
    m_areaIndex = (m_areaIndex + 1) % Pattern::k_numberOfSections;
  }

  // - Draw subcurves
  int n = f->numberOfSubCurves(true);
  for (int i = n - 1; i >= 0; i--) {
    Curve2DEvaluation<float> evaluationFloat = subCurveEvaluation<float>(f, i);
    CurveDrawing secondCurve(Curve2D(evaluationFloat, f), context(), tStart,
                             tEnd, tStep, f->subCurveColor(i), true,
                             f->properties().plotIsDotted());
    Curve2DEvaluation<double> evaluationDouble =
        subCurveEvaluation<double>(f, i);
    secondCurve.setPrecisionOptions(true, evaluationDouble, discontinuity);
    Curve2D pLower, pUpper;
    /* If the function only has 1 real subcurve (f->numberOfSubCurves(false)),
     * it means that the remaining subcurves (f->numberOfSubCurves(true)) are
     * the derivatives. */
    if (i == 0) {
      pLower = patternLower;
      pUpper = patternUpper;
    } else if (hasTwoCurves) {
      assert(i == 1);
      pLower = patternLower2;
    }
    secondCurve.setPatternOptions(pattern, patternStart, patternEnd, pLower,
                                  pUpper, patternWithoutCurve, axis);
    secondCurve.draw(this, ctx, rect);
  }
}

void GraphView::drawTangent(KDContext *ctx, KDRect rect,
                            Ion::Storage::Record record) const {
  if (!m_tangentDisplay || m_selectedRecord != record) {
    return;
  }
  ExpiringPointer<ContinuousFunction> f =
      functionStore()->modelForRecord(record);
  assert(f->canComputeTangent());
  /* TODO: We could handle tangent on second curve here by finding out
   * which of the two curves is selected. */
  float tangentParameterA = f->approximateSlope(m_cursor->t(), context());
  float tangentParameterB = -tangentParameterA * m_cursor->x() + m_cursor->y();

  /* To represent the tangent, we draw segment between the intersections
   * of the tangent and the drawnRect.
   *
   *        here
   *  _______x_____           _____________                    _____________
   * |      /      |         |             |             here x             |
   * |     /       | or here x-------------x and here or      |\            |
   * |____/________|         |_____________|                  |_\___________|
   *      x                                                     x
   * and here                                             and here
   *
   * These dots are taken instead of just taking the dots with the max and
   * min abscissa, in case the tangent is too vertical. Indeed, if the dots
   * are too far away outside of the current window, the tangent would be
   * drawn shifted away from the curve of the function because of
   * approximations errors.
   * */
  float minAbscissa =
      pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float maxAbscissa =
      pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  float minOrdinate =
      pixelToFloat(Axis::Vertical, rect.bottom() + k_externRectMargin);
  float maxOrdinate =
      pixelToFloat(Axis::Vertical, rect.top() - k_externRectMargin);

  Coordinate2D<float> leftIntersection(
      minAbscissa, tangentParameterA * minAbscissa + tangentParameterB);
  Coordinate2D<float> rightIntersection(
      maxAbscissa, tangentParameterA * maxAbscissa + tangentParameterB);
  int numberOfCandidateDots = 2;
  Coordinate2D<float> bottomIntersection(NAN, NAN);
  Coordinate2D<float> topIntersection(NAN, NAN);
  if (tangentParameterA != 0.) {
    bottomIntersection = Coordinate2D<float>(
        (minOrdinate - tangentParameterB) / tangentParameterA, minOrdinate);
    topIntersection = Coordinate2D<float>(
        (maxOrdinate - tangentParameterB) / tangentParameterA, maxOrdinate);
    numberOfCandidateDots += 2;
  }

  /* After computing the 4 intersections, choose the two that are visible
   * in the window to ensure their coordinates are not too far appart. */
  Coordinate2D<float> candidateDots[] = {leftIntersection, rightIntersection,
                                         bottomIntersection, topIntersection};
  Coordinate2D<float> firstVisibleDot;
  bool foundFirstVisibleDot = false;
  for (int i = 0; i < numberOfCandidateDots; i++) {
    Coordinate2D<float> currentDot = candidateDots[i];
    if (!currentDot.xIsIn(minAbscissa, maxAbscissa, true, true) ||
        !currentDot.yIsIn(minOrdinate, maxOrdinate, true, true)) {
      // Dot is not in window
      continue;
    }
    if (!foundFirstVisibleDot) {
      // First dot in window found
      firstVisibleDot = currentDot;
      foundFirstVisibleDot = true;
      continue;
    }
    // Second dot in window found
    drawSegment(ctx, rect, firstVisibleDot, currentDot, Palette::GrayVeryDark,
                false);
    break;
  }
}

static float polarThetaFromCoordinates(float x, float y,
                                       Preferences::AngleUnit angleUnit) {
  // Return θ, between -π and π in given angleUnit for a (x,y) position.
  return Trigonometry::ConvertRadianToAngleUnit<float>(
             std::arg(std::complex<float>(x, y)), angleUnit)
      .real();
}

void GraphView::drawPolar(KDContext *ctx, KDRect rect, ContinuousFunction *f,
                          float tStart, float tEnd, float tStep,
                          DiscontinuityTest discontinuity) const {
  assert(f->properties().isPolar());
  // Compute rect limits
  float rectLeft =
      pixelToFloat(Axis::Horizontal, rect.left() - k_externRectMargin);
  float rectRight =
      pixelToFloat(Axis::Horizontal, rect.right() + k_externRectMargin);
  float rectUp = pixelToFloat(Axis::Vertical, rect.top() - k_externRectMargin);
  float rectDown =
      pixelToFloat(Axis::Vertical, rect.bottom() + k_externRectMargin);

  const Preferences::AngleUnit angleUnit =
      Preferences::SharedPreferences()->angleUnit();
  const float piInAngleUnit = Trigonometry::PiInAngleUnit(angleUnit);
  /* Cancel optimization if :
   * - One of rect limits is nan.
   * - Step is too large, see cache optimization comments
   *   ("To optimize cache..."). */
  bool cancelOptimization =
      std::isnan(rectLeft + rectRight + rectUp + rectDown) ||
      tStep >= piInAngleUnit;

  bool rectOverlapsNegativeAbscissaAxis = false;
  if (cancelOptimization ||
      (rectUp > 0.0f && rectDown < 0.0f && rectLeft < 0.0f)) {
    if (cancelOptimization || rectRight > 0.0f) {
      // Origin is inside rect, tStart and tEnd cannot be optimized
      return drawFunction(ctx, rect, f, tStart, tEnd, tStep, discontinuity);
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
    return drawFunction(ctx, rect, f, tStart, tEnd, tStep, discontinuity);
  }

  /* The number of segments to draw can be reduced by drawing curve on intervals
   * where (tMin%π, tMax%π) intersects (tStart, tEnd).For instance :
   * if tStart=-π, tEnd=3π, tMin=π/4 and tMax=π/3, a curve is drawn between :
   * - [ π/4, π/3 ], [ 2π + π/4, 2π + π/3 ]
   * - [ -π + π/4, -π + π/3 ], [ π + π/4, π + π/3 ] in case f(θ) is negative */

  // 1 - Set offset so that tStart <= tMax+thetaOffset < piInAngleUnit+tStart
  float thetaOffset =
      std::ceil((tStart - tMax) / piInAngleUnit) * piInAngleUnit;

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
      drawFunction(ctx, rect, f, tCache1, tCache2, tStep, discontinuity);
    }
    thetaOffset += piInAngleUnit;
  }
}

void GraphView::drawFunction(KDContext *ctx, KDRect rect, ContinuousFunction *f,
                             float tStart, float tEnd, float tStep,
                             DiscontinuityTest discontinuity) const {
  assert(f->properties().isParametric() || f->properties().isInversePolar() ||
         f->properties().isPolar());
  CurveDrawing plot(Curve2D(evaluateXY<float>, f), context(), tStart, tEnd,
                    tStep, f->color());
  plot.setPrecisionOptions(false, nullptr, discontinuity);
  plot.draw(this, ctx, rect);
}

void GraphView::drawScatterPlot(KDContext *ctx, KDRect rect,
                                ContinuousFunction *f) const {
  assert(f->properties().isScatterPlot());
  // TODO Handle limiting tMax and tMin ?
  ApproximationContext approximationContext(context());
  for (Point p : f->iterateScatterPlot(context())) {
    drawDot(ctx, rect, k_dotSize, p.approximate2D<float>(approximationContext),
            f->color());
  }
}

void GraphView::resumePointsOfInterestDrawing() {
  m_computePointsOfInterest = true;
  m_interestView.dirtyBounds();
}

void GraphView::drawPointsOfInterest(KDContext *ctx, KDRect rect) {
  if (!hasFocus()) {
    return;
  }

  bool shouldComputePoints = m_computePointsOfInterest;
  m_computePointsOfInterest = false;

  Ion::Storage::Record selectedRec = selectedRecord();
  ExpiringPointer<ContinuousFunction> f =
      functionStore()->modelForRecord(selectedRec);
  if (!f->properties().isCartesian() ||
      functionWasInterrupted(
          functionStore()->indexOfRecordAmongActiveRecords(selectedRec))) {
    return;
  }

  PointsOfInterestCache *pointsOfInterestCache =
      App::app()->graphController()->pointsOfInterestForRecord(selectedRec);

  bool canDisplayPoints = pointsOfInterestCache->canDisplayPoints(m_interest);
  PointOfInterest p;
  int i = 0;
  do {
    // Compute more points of interest if necessary
    if (shouldComputePoints &&
        !pointsOfInterestCache->computeUntilNthPoint(i)) {
      // Computation was interrupted
      return;
    }

    if (i >= pointsOfInterestCache->numberOfPoints()) {
      return;
    }

    p = pointsOfInterestCache->pointAtIndex(i);
    bool wasAlreadyDrawn = i < m_nextPointOfInterestIndex;
    i++;
    if (!wasAlreadyDrawn) {
      m_nextPointOfInterestIndex = i;
    }

    if (m_interest != Poincare::Solver<double>::Interest::None &&
        m_interest != p.interest()) {
      continue;
    }

    if (canDisplayPoints &&
        !pointsOfInterestCache->canDisplayPoints(m_interest)) {
      canDisplayPoints = false;
      // Hide the interest points by redrawing everything but them.
      if (cursorView()) {
        cursorView()->setCursorFrame(this, cursorFrame(), true);
      }
      // Redraw curve and cursor without any interest point
      drawRect(ctx, rect);
      if (cursorView()) {
        cursorView()->setHighlighted(false);
        static_cast<MemoizedCursorView *>(cursorView())
            ->redrawCursor(rect.translatedBy(absoluteOrigin()));
      }
    }

    if (!canDisplayPoints) {
      // Do not display anything but keep computing more points of interest.
      continue;
    }

    // Draw the dot
    Coordinate2D<float> dotCoordinates =
        static_cast<Coordinate2D<float>>(p.xy());

    KDRect dotRelativeRect = dotRect(k_dotSize, dotCoordinates);
    /* If the dot intersects the dirty rect, force the redraw.
     * Either dotRelativeRect or dirtyRect needs to be translated, as one is
     * relative and the other absolute. Since dotRect might have been clamped to
     * KDCOORDINATE_MAX, translating dirtyRect is safer. */
    if (!dotRelativeRect.intersects(
            dirtyRect().translatedBy(absoluteOrigin().opposite())) &&
        wasAlreadyDrawn) {
      continue;
    }
    // If the dot is below the cursor, erase the cursor and redraw it
    KDRect frameOfCursor = cursorView() ? cursorFrame() : KDRectZero;
    bool redrawCursor = frameOfCursor.intersects(dotRelativeRect);
    if (redrawCursor) {
      // Erase cursor and make rect dirty
      assert(cursorView());
      cursorView()->setCursorFrame(this, frameOfCursor, true);
    }
    drawDot(ctx, rect, k_dotSize, dotCoordinates, Escher::Palette::GrayDarkest);
    if (redrawCursor) {
      /* WARNING: We cannot assert that cursorView is a MemoizedCursorView
       * but it is thanks to the constructor. */
      /* The cursor cannot be safely highlighted here since it might intersect
       * the dot of the point of interest without being exactly on it. */
      static_cast<MemoizedCursorView *>(cursorView())
          ->redrawCursor(rect.translatedBy(absoluteOrigin()));
    }
  } while (1);
}

KDRect GraphView::boundsWithoutBanner() const {
  assert(m_banner);
  return KDRect(0, 0, bounds().width(),
                bounds().height() - m_banner->bounds().height());
}

void GraphView::drawAxesAndGrid(KDContext *ctx, KDRect rect) const {
  InteractiveCurveViewRange *viewRange =
      static_cast<InteractiveCurveViewRange *>(range());
  if (viewRange->gridType() == InteractiveCurveViewRange::GridType::Polar) {
    PlotPolicy::WithPolarGrid::DrawGrid(this, ctx, rect);

  } else {
    assert(viewRange->gridType() ==
           InteractiveCurveViewRange::GridType::Cartesian);
    PlotPolicy::WithCartesianGrid::DrawGrid(this, ctx, rect);
  }
  drawAxes(this, ctx, rect);
}

}  // namespace Graph
