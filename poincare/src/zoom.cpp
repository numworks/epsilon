#include <poincare/zoom.h>
#include <poincare/trinary_boolean.h>
#include <poincare/solver_algorithms.h>
#include <string.h>

namespace Poincare {

// HorizontalAsymptoteHelper

void Zoom::HorizontalAsymptoteHelper::update(Coordinate2D<float> p, float slope) {
  Coordinate2D<float> * bound = p.x1() < m_center ? &m_left : &m_right;
  slope = std::fabs(slope);
  if (!std::isfinite(slope)) {
    return;
  }
  if (std::isnan(bound->x1())) {
    if (slope < k_threshold - k_hysteresis) {
      *bound = p;
    }
  } else if (slope > k_threshold + k_hysteresis) {
    *bound = Coordinate2D<float>();
  }
}

// Zoom - Public

Range2D Zoom::Sanitize(Range2D range, float normalYXRatio, float maxFloat) {
  /* Values for tMin and tMax actually do not matter here, as no function will
   * be evaluated to generate this zoom. */
  Zoom zoom(-maxFloat, maxFloat, normalYXRatio, nullptr, maxFloat);
  zoom.m_interestingRange = range;
  return zoom.range(false, false);
}

Range2D Zoom::range(bool beautify, bool forceNormalization) const {
  Range2D result;
  Range2D pretty = beautify ? prettyRange(forceNormalization) : sanitizedRange();
  result.x()->setMin(pretty.xMin(), m_maxFloat);
  result.x()->setMax(pretty.xMax(), m_maxFloat);
  result.y()->setMin(pretty.yMin(), m_maxFloat);
  result.y()->setMax(pretty.yMax(), m_maxFloat);


  assert(!m_forcedRange.x()->isValid() ||
         (result.xMin() == m_forcedRange.xMin() && result.xMax() == m_forcedRange.xMax()));
  assert(!m_interestingRange.x()->isValid() ||
         ((result.xMin() <= m_interestingRange.xMin() + Float<float>::Epsilon() * std::fabs(m_interestingRange.xMin()) || !std::isfinite(m_interestingRange.xMin())) &&
          (m_interestingRange.xMax() - Float<float>::Epsilon() * std::fabs(m_interestingRange.xMax()) <= result.xMax()|| !std::isfinite(m_interestingRange.xMax()))) ||
         m_forcedRange.x()->isValid());

  assert(!m_forcedRange.y()->isValid() ||
         (result.yMin() == m_forcedRange.yMin() && result.yMax() == m_forcedRange.yMax()));
  assert(!m_interestingRange.y()->isValid() ||
         ((result.yMin() <= m_interestingRange.yMin() + Float<float>::Epsilon() * std::fabs(m_interestingRange.yMin()) || !std::isfinite(m_interestingRange.yMin())) &&
          (m_interestingRange.yMax() - Float<float>::Epsilon() * std::fabs(m_interestingRange.yMax()) <= result.yMax() || !std::isfinite(m_interestingRange.yMax()))) ||
         m_forcedRange.y()->isValid());

  assert(result.x()->isValid() && result.y()->isValid() && !result.x()->isEmpty() && !result.y()->isEmpty());
  return result;
}

static Range1D computeNewBoundsAfterZoomingOut(float t, Range1D oldRange, float minMargin, float maxMargin, float limit) {
  oldRange.extend(t, limit);
  /* When we zoom out, we want to recompute both the xMin and xMax so that
   * previous values that where within margins bounds stay in it, even if
   * the xRange increased.
   *
   * |-------------|----------------------------|------------|
   * ^new min (X)                                            ^new max (Y)
   *               ^old min (A)                 ^old max (B)
   *        ^min margin (r)                            ^max margin (R)
   *
   * We have to solve the equation system:
   * Y - B = R * (Y - X)
   * A - X = r * (Y - X)
   *
   * We find the formulas:
   * X = A - (r * (B - A) / (1 - (R + r)))
   * Y = B + (R * (B - A) / (1 - (R + r)))
   */
  assert(minMargin + maxMargin < 1.f);
  float k = oldRange.length() / (1.f - (minMargin + maxMargin));
  float newMin = oldRange.min() - k * minMargin;
  float newMax = oldRange.max() + k * maxMargin;
  return Range1D(newMin, newMax);
}

void Zoom::fitPoint(Coordinate2D<float> xy, bool flipped, float leftMargin, float rightMargin, float bottomMargin, float topMargin) {
  float xL = m_interestingRange.x()->length(), yL = m_interestingRange.y()->length();
  Range1D xRWithoutMargins(m_interestingRange.xMin() + leftMargin * xL, m_interestingRange.xMax() - rightMargin * xL);
  Range1D yRWithoutMargins(m_interestingRange.yMin() + bottomMargin * yL, m_interestingRange.yMax() - topMargin * yL);
  Range1D xR = computeNewBoundsAfterZoomingOut(xy.x1(), xRWithoutMargins, leftMargin, rightMargin, m_maxFloat);
  Range1D yR = computeNewBoundsAfterZoomingOut(xy.x2(), yRWithoutMargins, bottomMargin, topMargin, m_maxFloat);
  privateFitPoint(Coordinate2D<float>(xR.min(), yR.min()), flipped);
  privateFitPoint(Coordinate2D<float>(xR.max(), yR.max()), flipped);
}

void Zoom::fitFullFunction(Function2DWithContext<float> f, const void * model) {
  float step = m_bounds.length() / (k_sampleSize - 1);
  for (size_t i = 0; i < k_sampleSize; i++) {
    float t = m_bounds.min() + step * i;
    privateFitPoint(f(t, model, m_context));
  }
}

static Solver<float>::Interest pointIsInterestingHelper(Coordinate2D<float> a, Coordinate2D<float> b, Coordinate2D<float> c, const void * aux) {
  Solver<float>::BracketTest tests[] = { Solver<float>::OddRootInBracket, Solver<float>::MinimumInBracket, Solver<float>::MaximumInBracket, Solver<float>::DiscontinuityInBracket };
  Solver<float>::Interest interest = Solver<float>::Interest::None;
  for (Solver<float>::BracketTest & test : tests) {
    interest = test(a, b, c, aux);
    if (interest != Solver<float>::Interest::None) {
      break;
    }
  }
  return interest;
}

void Zoom::fitPointsOfInterest(Function2DWithContext<float> f, const void * model, bool vertical, Function2DWithContext<double> fDouble) {
  HorizontalAsymptoteHelper asymptotes(m_bounds.center());
  float (Coordinate2D<float>::*ordinate)() const = vertical ? &Coordinate2D<float>::x1 : &Coordinate2D<float>::x2;
  double (Coordinate2D<double>::*ordinateDouble)() const = vertical ? &Coordinate2D<double>::x1 : &Coordinate2D<double>::x2;
  InterestParameters params = { .f = f, .fDouble = fDouble, .model = model, .context = m_context, .asymptotes = &asymptotes, .ordinate = ordinate, .ordinateDouble = ordinateDouble };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const InterestParameters * p = static_cast<const InterestParameters *>(aux);
    return (p->f(t, p->model, p->context).*p->ordinate)();
  };
  Solver<double>::FunctionEvaluation evaluatorDouble = [](double t, const void * aux) {
    const InterestParameters * p = static_cast<const InterestParameters *>(aux);
    return (p->fDouble(t, p->model, p->context).*p->ordinateDouble)();
  };
  bool leftInterrupted, rightInterrupted;
  fitWithSolver(&leftInterrupted, &rightInterrupted, evaluator, &params, PointIsInteresting, HonePoint, vertical, evaluatorDouble, pointIsInterestingHelper);
  /* If the search has been interrupted, the curve is supposed to have an
   * infinite number of points in this direction. An horizontal asymptote
   * would be the result of a sampling artifact and can be discarded. */
  if (!leftInterrupted) {
    privateFitPoint(asymptotes.left(), vertical);
  }
  if (!rightInterrupted) {
    privateFitPoint(asymptotes.right(), vertical);
  }
}

void Zoom::fitIntersections(Function2DWithContext<float> f1, const void * model1, Function2DWithContext<float> f2, const void * model2, bool vertical) {
  /* TODO Function x=f(y) are not intersectable right now, there is no need to
   * handle this case yet. */
  assert(!vertical);
  IntersectionParameters params = { .f1 = f1, .f2 = f2, .model1 = model1, .model2 = model2, .context = m_context };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const IntersectionParameters * p = static_cast<const IntersectionParameters *>(aux);
    return p->f1(t, p->model1, p->context).x2() - p->f2(t, p->model2, p->context).x2();
  };
  bool dummy;
  fitWithSolver(&dummy, &dummy, evaluator, &params, Solver<float>::OddRootInBracket, HoneIntersection, vertical);
}

void Zoom::fitConditions(PiecewiseOperator p, Function2DWithContext<float> fullFunction, const void * model, const char * symbol, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool vertical) {
  struct ConditionsParameters {
    Zoom * zoom;
    PiecewiseOperator p;
    const char * symbol;
    Preferences::ComplexFormat complexFormat;
    Preferences::AngleUnit angleUnit;
    Function2DWithContext<float> fullFunction;
    const void * model;
    bool vertical;
  };
  const ConditionsParameters params = { .zoom = this, .p = p, .symbol = symbol, .complexFormat = complexFormat, .angleUnit = angleUnit, .fullFunction = fullFunction, .model = model, .vertical = vertical };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const ConditionsParameters * params = static_cast<const ConditionsParameters *>(aux);
    return static_cast<float>(params->p.indexOfFirstTrueConditionWithValueForSymbol(params->symbol, t, params->zoom->m_context, params->complexFormat, params->angleUnit));
  };
  Solver<float>::BracketTest test = [](Coordinate2D<float> a, Coordinate2D<float>, Coordinate2D<float> c, const void *) {
    return Solver<float>::BoolToInterest(a.x2() != c.x2(), Solver<float>::Interest::Discontinuity);
  };
  Solver<float>::HoneResult hone = [](Solver<float>::FunctionEvaluation, const void * aux, float a, float b, Solver<float>::Interest, float) {
    const ConditionsParameters * params = static_cast<const ConditionsParameters *>(aux);
    params->zoom->fitPoint(params->fullFunction(a, params->model, params->zoom->m_context), params->vertical);
    return params->fullFunction(b, params->model, params->zoom->m_context);
  };
  bool dummy;
  fitWithSolver(&dummy, &dummy, evaluator, &params, test, hone, vertical);
}

void Zoom::fitMagnitude(Function2DWithContext<float> f, const void * model, bool vertical) {
  /* We compute the log mean value of the expression, which gives an idea of the
   * order of magnitude of the function, to crop the Y axis. */
  constexpr float aboutZero = Solver<float>::k_minimalAbsoluteStep;
  Range1D sample;
  float nSum = 0.f, pSum = 0.f;
  int nPop = 0, pPop = 0;

  float (Coordinate2D<float>::*ordinate)() const = vertical ? &Coordinate2D<float>::x1 : &Coordinate2D<float>::x2;
  Range2D saneRange = sanitizedRange();
  Range1D xRange = *(vertical ? saneRange.y() : saneRange.x());
  float step = xRange.length() / (k_sampleSize - 1);

  for (size_t i = 0; i < k_sampleSize; i++) {
    float x = xRange.min() + i * step;
    float y = (f(x, model, m_context).*ordinate)();
    sample.extend(y, m_maxFloat);
    float yAbs = std::fabs(y);
    if (!(yAbs > aboutZero)) { // Negated to account for NANs
      continue;
    }
    float yLog = std::log(yAbs);
    if (y < 0.f) {
      nSum += yLog;
      nPop++;
    } else {
      pSum += yLog;
      pPop++;
    }
  }
  Range1D * magnitudeRange = vertical ? m_magnitudeRange.x() : m_magnitudeRange.y();
  float yMax = pPop > 0 ? std::min(sample.max(), std::exp(pSum / pPop  + 1.f)) : sample.max();
  magnitudeRange->extend(yMax, m_maxFloat);
  float yMin = nPop > 0 ? std::max(sample.min(), -std::exp(nSum / nPop  + 1.f)) : sample.min();
  magnitudeRange->extend(yMin, m_maxFloat);
}

void Zoom::fitBounds(Function2DWithContext<float> f, const void * model, bool vertical) {
  float tMin = m_bounds.min(), tMax = m_bounds.max();
  if (std::fabs(tMin) >= m_maxFloat || std::fabs(tMax) >= m_maxFloat) {
    return;
  }
  // Fit the middle of the interval if it's finite
  float tMiddle = (tMin + tMax) / 2;
  Coordinate2D<float> middle(f(tMiddle, model, m_context));
  Coordinate2D<float> pointToFit = vertical ? Coordinate2D<float>(tMiddle, middle.x1()) : Coordinate2D<float>(tMiddle, middle.x2());
  privateFitPoint(pointToFit, vertical);

 /* Set the default half length in case the middle is the only point
  * in the interesting range */
  float halfLength = (tMax - tMin) / 2;
  m_defaultHalfLength = std::min(m_defaultHalfLength, halfLength);
}

// Zoom - Private

Solver<float>::Interest Zoom::PointIsInteresting(Coordinate2D<float> a, Coordinate2D<float> b, Coordinate2D<float> c, const void * aux) {
  const InterestParameters * params = static_cast<const InterestParameters *>(aux);
  float slope = (c.x2() - a.x2()) / (c.x1() - a.x1());
  params->asymptotes->update(c, slope);
  Solver<float>::Interest res = pointIsInterestingHelper(a, b, c, aux);
  /* Filter out variations that are caused by:
   * - loss of significance when outputting very small values compared to the
   *   parameter.
   * - rounding errors, when two similar values are rounded in different
   *   directions.
   * The tolerance is chosen to be as small as possible while still being large
   * enough to give good results in practice. Since callers of Zoom do not rely
   * on high levels of precision, this can be increased if need be.
   * FIXME Tolerance for detecting approximation errors should be computed by
   * analysing the input expression. */
  constexpr float k_tolerance = 4.f * Solver<float>::k_relativePrecision;
  if ((res == Solver<float>::Interest::LocalMinimum || res == Solver<float>::Interest::LocalMaximum)
   && (std::fabs((a.x2() - b.x2()) / b.x1()) < k_tolerance || std::fabs((a.x2() - b.x2()) / b.x2()) < k_tolerance)) {
    return Solver<float>::Interest::None;
  }
  return res;
}

static void honeHelper(Solver<float>::FunctionEvaluation f, const void * aux, float a, float b, Solver<float>::Interest interest, Solver<float>::BracketTest test, Coordinate2D<float> * pa, Coordinate2D<float> * pu, Coordinate2D<float> * pv, Coordinate2D<float> * pb) {
  /* Use a simple dichotomy in [a,b] to hone in on the point of interest
   * without using the costly Brent methods. */
  constexpr int k_numberOfIterations = 9; // TODO Tune
  constexpr float k_goldenRatio = static_cast<float>(SolverAlgorithms::k_goldenRatio);

  /* Define two points u and v such that a < u < v < b. Then, we can
   * determine wether the point of interest exists on [a,v] or [u,b].
   * We use the golden ratio to split the range as it has the properties of
   * keeping the ratio between iterations while only recomputing one point. */
  float u = a + k_goldenRatio * (b - a);
  float v = b - (u - a);
  *pa = Coordinate2D<float>(a, f(a, aux));
  *pb = Coordinate2D<float>(b, f(b, aux));
  *pu = Coordinate2D<float>(u, f(u, aux));
  *pv = Coordinate2D<float>(v, f(v, aux));

  for (int i = 0; i < k_numberOfIterations; i++) {
    /* Select the interval that contains the point of interest. If, because of
     * some artifacts, both or neither contains a point, we favor the interval
     * on the far side (i.e. [m,b]) to avoid finding the same point twice. */
    if (test(*pu, *pv, *pb, aux) != Solver<float>::Interest::None) {
      *pa = *pu;
      *pu = *pv;
      float newV = pb->x1() - (pu->x1() - pa->x1());
      *pv = Coordinate2D<float>(newV, f(newV, aux));
    } else if (test(*pa, *pu, *pv, aux) != Solver<float>::Interest::None) {
      *pb = *pv;
      *pv = *pu;
      float newU = pa->x1() + (pb->x1() - pv->x1());
      *pu = Coordinate2D<float>(newU, f(newU, aux));
    } else {
      break;
    }
  }
}

Coordinate2D<float> Zoom::HonePoint(Solver<float>::FunctionEvaluation f, const void * aux, float a, float b, Solver<float>::Interest interest, float precision) {
  Coordinate2D<float> pa, pu, pv, pb;
  honeHelper(f, aux, a, b, interest, pointIsInterestingHelper, &pa, &pu, &pv, &pb);

  constexpr float k_tolerance = 1.f / Solver<float>::k_relativePrecision;
  /* Most functions will taper off near a local extremum. If the slope
   * diverges, it is more likely we have found an even vertical asymptote. */
  bool discontinuous = (interest == Solver<float>::Interest::LocalMinimum || interest == Solver<float>::Interest::LocalMaximum)
    && (std::max((pu.x2() - pa.x2()) / (pu.x1() - pa.x1()), (pv.x2() - pb.x2()) / (pv.x1() - pb.x1())) > k_tolerance);
  /* If the function is discontinuous around the solution (e.g. 1/x^2), we
   * discard the y value to avoid zooming in on diverging points. */
  return Coordinate2D<float>(pb.x1(), interest == Solver<float>::Interest::Root ? 0.f : discontinuous ? NAN : pb.x2());
}

Coordinate2D<float> Zoom::HoneIntersection(Solver<float>::FunctionEvaluation f, const void * aux, float a, float b, Solver<float>::Interest interest, float precision) {
  Coordinate2D<float> pa, pu, pv, pb;
  honeHelper(f, aux, a, b, interest, Solver<float>::EvenOrOddRootInBracket, &pa, &pu, &pv, &pb);

  /* We must make sure the "root" we've found is not an odd vertical asymptote.
   * We only select roots that are lower than an arbitrary threshold. The
   * value of the threshold itself does not require fine tuning as the
   * functions we want to filter out will diverge most of the time.
   * FIXME This test will fail when confronted with discontinuous functions
   * that do not have asymptotes. */
  constexpr float k_threshold = 1.f;
  if (!(std::fabs(pb.x2()) < k_threshold)) {
    return Coordinate2D<float>();
  }

  const IntersectionParameters * p = static_cast<const IntersectionParameters *>(aux);
  return p->f1(pb.x1(), p->model1, p->context);
}

static Range1D sanitationHelper(Range1D range, float defaultHalfLength) {
  if (!range.isValid()) {
    range = Range1D(0.f, 0.f);
  }
  if (range.isEmpty()) {
    float c = range.min();
    range = Range1D(c - defaultHalfLength, c + defaultHalfLength);
  }
  return range;
}

Range2D Zoom::sanitize2DHelper(Range2D range) const {
  Range1D xRange = m_forcedRange.x()->isValid() ? *m_forcedRange.x() : sanitationHelper(*range.x(), m_defaultHalfLength);
  Range1D yRange = m_forcedRange.y()->isValid() ? *m_forcedRange.y() : sanitationHelper(*range.y(), xRange.length() * 0.5f * m_normalRatio);
  return Range2D(xRange, yRange);
}

Range2D Zoom::prettyRange(bool forceNormalization) const {
  assert(!forceNormalization || !m_forcedRange.x()->isValid() || !m_forcedRange.y()->isValid());

  Range2D saneRange = m_interestingRange;
  saneRange.extend(Coordinate2D<float>(m_magnitudeRange.xMin(), m_magnitudeRange.yMin()), m_maxFloat);
  saneRange.extend(Coordinate2D<float>(m_magnitudeRange.xMax(), m_magnitudeRange.yMax()), m_maxFloat);
  saneRange = sanitize2DHelper(saneRange);

  float xLength = saneRange.x()->length();
  float yLength = saneRange.y()->length();
  float xLengthNormalized = yLength / m_normalRatio;
  float yLengthNormalized = xLength * m_normalRatio;
  constexpr float k_minimalXCoverage = 0.3f;
  constexpr float k_minimalXNormalizedCoverage = 0.15f;
  constexpr float k_minimalYCoverage = 0.3f;
  constexpr float k_minimalYNormalizedCoverage = 0.15f;

  /* Y can be normalized if:
   * - the Y range (interesting + magnitude) makes up for at least 30% of the
   *   normalized Y range (i.e. the curve does not appear squeezed).
   * - the normalized Y range makes up for at least 15% of the Y range. This is
   *   to prevent that, by shrinking Y, the X axis becomes too long for the
   *   remaining visible part of the curve.
   * - a normalized Y range can fit the interesting Y range. We only count the
   *   interesting Y range for this part as discarding the part that comes from
   *   the magnitude is not an issue.*/

  bool xLengthCompatibleWithNormalization =
    xLengthNormalized * k_minimalXCoverage <= xLength &&
    xLength * k_minimalXNormalizedCoverage <= xLengthNormalized &&
    m_interestingRange.x()->length() <= xLengthNormalized;
  bool yLengthCompatibleWithNormalization =
    yLengthNormalized * k_minimalYCoverage <= yLength &&
    yLength * k_minimalYNormalizedCoverage <= yLengthNormalized &&
    m_interestingRange.y()->length() <= yLengthNormalized;

  bool normalizeX = !m_forcedRange.x()->isValid() && (forceNormalization || xLengthCompatibleWithNormalization);
  bool normalizeY = !m_forcedRange.y()->isValid() && (forceNormalization || yLengthCompatibleWithNormalization);
  if (normalizeX && normalizeY) {
    /* Both axes are good candidates for normalization, pick the one that does
     * not lead to the range being shrunk. */
    normalizeX = xLength < xLengthNormalized;
    normalizeY = yLength < yLengthNormalized;
  }
  if (!(normalizeX || normalizeY)) {
    return saneRange;
  }
  assert(normalizeX != normalizeY);

  Range1D * rangeToEdit;
  const Range1D * interestingRange;
  float normalLength;
  if (normalizeX) {
    rangeToEdit = saneRange.x();
    interestingRange = m_interestingRange.x();
    normalLength = xLengthNormalized;
  } else {
    rangeToEdit = saneRange.y();
    interestingRange = m_interestingRange.y();
    normalLength = yLengthNormalized;
  }

  float interestingCenter = interestingRange->isValid() ? interestingRange->center() : rangeToEdit->center();
  assert(std::isfinite(interestingCenter));
  float portionOverInterestingCenter = (rangeToEdit->max() - interestingCenter) / rangeToEdit->length();
  float lengthOverCenter = portionOverInterestingCenter * normalLength;
  float lengthUnderCenter = normalLength - lengthOverCenter;
  if (interestingRange->isValid() && interestingCenter - lengthUnderCenter > interestingRange->min()) {
    *rangeToEdit = Range1D(interestingRange->min(), interestingRange->min() + normalLength);
  } else if (interestingRange->isValid() && interestingCenter + lengthOverCenter < interestingRange->max()) {
    *rangeToEdit = Range1D(interestingRange->max() - normalLength, interestingRange->max());
  } else {
    *rangeToEdit = Range1D(interestingCenter - lengthUnderCenter, interestingCenter + lengthOverCenter);
  }

  return saneRange;
}

void Zoom::fitWithSolver(bool * leftInterrupted, bool * rightInterrupted, Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test, Solver<float>::HoneResult hone, bool vertical, Solver<double>::FunctionEvaluation fDouble, Solver<float>::BracketTest testForCenterOfInterval) {
  assert(leftInterrupted && rightInterrupted);

  /* Pick margin large enough to detect an extremum around zero, for some
   * particularly flat function such as (x+10)(x-10). */
  constexpr float k_marginAroundZero = 1e-2f;

  float c = m_bounds.center();
  float d = std::max(k_marginAroundZero, std::fabs(c * Solver<float>::k_relativePrecision));
  *rightInterrupted = fitWithSolverHelper(c + d, m_bounds.max(), evaluator, aux, test, hone, vertical, fDouble);
  *leftInterrupted = fitWithSolverHelper(c - d, m_bounds.min(), evaluator, aux, test, hone, vertical, fDouble);

  Coordinate2D<float> p1(c - d, evaluator(c - d, aux));
  Coordinate2D<float> p2(c, evaluator(c, aux));
  Coordinate2D<float> p3(c + d, evaluator(c + d, aux));
  Solver<float>::Interest centerInterest = testForCenterOfInterval != nullptr ? testForCenterOfInterval(p1, p2, p3, aux) : test(p1, p2, p3, aux);
  if (centerInterest != Solver<float>::Interest::None) {
    privateFitPoint(p2, vertical);
  }
}

bool Zoom::fitWithSolverHelper(float start, float end, Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test, Solver<float>::HoneResult hone, bool vertical, Solver<double>::FunctionEvaluation fDouble) {
  constexpr int k_maxPointsOnOneSide = 20;
  constexpr int k_maxPointsIfInfinite = 5;

  /* Search for points of interest in one direction, up to a certain number.
   * - k_maxPointsOnOneSide is the absolute maximum number of points we are
   *   allowed to find. It is high enough to correctly zoom on a tenth degree
   *   polynomial.
   * - if we find more the k_maxPointsOnOneSide points, we assume that there
   *   are an infinite number of points. As such there is no need to display
   *   all of them, and we only register up to k_maxPointsIfInfinite. This
   *   trick improves the display of periodic function, which would otherwise
   *   appear cramped. */

  Solver<float> solver(start, end);
  Range2D tempRange;
  int n = 0;
  Coordinate2D<float> p;
  while (std::isfinite((p = solver.next(evaluator, aux, test, hone)).x1())) { // assignment in condition
    if (fDouble != nullptr &&
        solver.lastInterest() == Solver<float>::Interest::Discontinuity &&
        std::isnan(p.x2()) &&
        std::isfinite(fDouble(p.x1(), aux)))
    {
      /* The function evaluates to NAN in single-precision only. It is likely
        * we have reached the limits of the float type, such as when
        * evaluating y=(e^x-1)/(e^x+1) for x~90 (which leads to ∞/∞). */
      return false;
    }
    privateFitPoint(p, vertical);
    n++;
    if (n == k_maxPointsIfInfinite) {
      tempRange = m_interestingRange;
    } else if (n >= k_maxPointsOnOneSide) {
      m_interestingRange = tempRange;
      return true;
    }
  }
  return false;
}

void Zoom::privateFitPoint(Coordinate2D<float> xy, bool flipped) {
  m_interestingRange.extend(flipped ? Coordinate2D<float>(xy.x2(), xy.x1()) : xy, m_maxFloat);
}

}
