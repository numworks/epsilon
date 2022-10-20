#include <poincare/zoom.h>
#include <string.h>

namespace Poincare {

// Public

Range2D Zoom::Sanitize(Range2D range, float normalYXRatio, float maxFloat) {
  Zoom zoom(-maxFloat, maxFloat, normalYXRatio, nullptr);
  zoom.m_interestingRange = range;
  return zoom.range(maxFloat, false);
}

 Range2D Zoom::range(float maxFloat, bool forceNormalization) const {
  Range2D result;
  Range2D pretty = prettyRange();
  result.x().setMin(pretty.xMin(), maxFloat);
  result.x().setMax(pretty.xMax(), maxFloat);
  result.y().setMin(pretty.yMin(), maxFloat);
  result.y().setMax(pretty.yMax(), maxFloat);

  if (forceNormalization) {
    result.setRatio(m_normalRatio, false);
  }

  return result;
}

void Zoom::fitFullFunction(Function2DWithContext f, const void * model) {
  float step = m_bounds.length() / (k_sampleSize - 1);
  for (size_t i = 0; i < k_sampleSize; i++) {
    float t = m_bounds.min() + step * i;
    m_interestingRange.extend(f(t, model, m_context));
  }
}

void Zoom::fitPointsOfInterest(Function2DWithContext f, const void * model) {
  struct Parameters {
    Function2DWithContext f;
    const void * model;
    Context * context;
  };
  Parameters params = { .f = f, .model = model, .context = m_context };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const Parameters * p = static_cast<const Parameters *>(aux);
    return p->f(t, p->model, p->context).x2(); // TODO Zoom could also work for x=f(y) functions
  };
  fitWithSolver(evaluator, &params, PointIsInteresting);
}

void Zoom::fitIntersections(Function2DWithContext f1, const void * model1, Function2DWithContext f2, const void * model2) {
  struct Parameters {
    Function2DWithContext f1;
    Function2DWithContext f2;
    const void * model1;
    const void * model2;
    Context * context;
  };
  Parameters params = { .f1 = f1, .f2 = f2, .model1 = model1, .model2 = model2, .context = m_context };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const Parameters * p = static_cast<const Parameters *>(aux);
    return p->f1(t, p->model1, p->context).x2() - p->f2(t, p->model2, p->context).x2();
  };
  fitWithSolver(evaluator, &params, Solver<float>::EvenOrOddRootInBracket);
}

void Zoom::fitMagnitude(Function2DWithContext f, const void * model) {
  /* We comute the log mean value of the expression, which gives an idea of the
   * order of magnitude of the function, to crop the Y axis. */
  constexpr float aboutZero = Solver<float>::k_minimalAbsoluteStep;
  Range1D sample;
  float nSum = 0.f, pSum = 0.f;
  int nPop = 0, pPop = 0;

  Range1D xRange = sanitizedXRange();
  float step = xRange.length() / (k_sampleSize - 1);
  for (int i = 0; i < k_sampleSize; i++) {
    float x = xRange.min() + i * step;
    float y = f(x, model, m_context).x2(); // TODO Zoom could also work for x=f(y) functions
    sample.extend(y);
    float yAbs = std::fabs(y);
    if (yAbs <= aboutZero) {
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
  float yMax = pPop > 0 ? std::min(sample.max(), std::exp(pSum / pPop  + 1.f)) : sample.max();
  m_magnitudeYRange.extend(yMax);
  float yMin = nPop > 0 ? std::max(sample.min(), -std::exp(nSum / nPop  + 1.f)) : sample.min();
  m_magnitudeYRange.extend(yMin);
}

// Private

Solver<float>::Interest Zoom::PointIsInteresting(float ya, float yb, float yc) {
  Solver<float>::BracketTest tests[] = { Solver<float>::OddRootInBracket, Solver<float>::MinimumInBracket, Solver<float>::MaximumInBracket, Solver<float>::DiscontinuityInBracket };
  Solver<float>::Interest interest;
  for (Solver<float>::BracketTest & test : tests) {
    interest = test(ya, yb, yc);
    if (interest != Solver<float>::Interest::None) {
      break;
    }
  }
  return interest;
}

Range1D Zoom::sanitizedXRange() const {
  Range2D thisRange = m_interestingRange; // Copy for const-ness
  if (!thisRange.x().isValid()) {
    assert(!thisRange.y().isValid());
    return Range1D(-Range1D::k_defaultHalfLength, Range1D::k_defaultHalfLength);
  }
  if (thisRange.x().isEmpty()) {
    float c = thisRange.x().min();
    return Range1D(c - Range1D::k_defaultHalfLength, c + Range1D::k_defaultHalfLength);
  }
  // FIXME Add margin around interesting range ?
  return thisRange.x();
}

Range2D Zoom::prettyRange() const {
  Range2D thisRange = m_interestingRange; // Copy for const-ness
  Range1D xRange = sanitizedXRange();

  Range1D yRange = thisRange.y();
  yRange.extend(m_magnitudeYRange.min());
  yRange.extend(m_magnitudeYRange.max());

  float xLength = xRange.length();
  float yLength = yRange.length();
  float xLengthNormalized = yLength / m_normalRatio;
  float yLengthNormalized = xLength * m_normalRatio;
  constexpr float k_minimalXCoverage = 0.8f;
  constexpr float k_minimalYCoverage = 0.3f;
  bool yCanBeNormalized = yLengthNormalized * k_minimalYCoverage <= yLength && yLength <= yLengthNormalized;
  if (!yRange.isValid() || yLength == 0.f || yCanBeNormalized) {
    float yCenter = yRange.center();
    if (!std::isfinite(yCenter)) {
      yCenter = 0.f;
    }
    yRange = Range1D(yCenter - 0.5f * yLengthNormalized, yCenter + 0.5f * yLengthNormalized);
  } else if (xLengthNormalized * k_minimalXCoverage <= xLength && xLength <= xLengthNormalized) {
    float xCenter = xRange.center();
    xRange = Range1D(xCenter - xLengthNormalized * 0.5f, xCenter + xLengthNormalized * 0.5f);
  }

  assert(xRange.isValid() && !xRange.isEmpty() && yRange.isValid() && !yRange.isEmpty());
  return Range2D(xRange, yRange);
}

void Zoom::fitWithSolver(Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test) {
  /* Step away from the center, as it is more likely to be a significant value
   * (typically zero).*/
  float c = m_bounds.center();
  float d = std::max(Solver<float>::k_minimalAbsoluteStep, std::fabs(c * Solver<float>::k_relativePrecision));
  fitWithSolverHelper(c - d, m_bounds.max(), evaluator, aux, test);
  fitWithSolverHelper(c - d, m_bounds.min(), evaluator, aux, test);
}

void Zoom::fitWithSolverHelper(float start, float end, Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test) {
  Solver<float> solver(start, end);
  Coordinate2D<float> p = solver.next(evaluator, aux, test, SelectFar);
  int n = 0;
  while (n++ < k_maxPointsOnOneSide && solver.lastInterest() != Solver<float>::Interest::None) {
    m_interestingRange.extend(p);
    p = solver.next(evaluator, aux, test, SelectFar);
  }
}


#if 0
// Public

static Range1D sanitationHelper(Range1D range, float normalHalfLength) {
  if (!range.isValid()) {
    range = Range1D(0.f, 0.f);
  }
  if (range.isEmpty()) {
    range = Range1D(range.min() - normalHalfLength, range.max() + normalHalfLength);
  }
  assert(range.min() < range.max());
  return range;
}

Range2D Zoom::Sanitize(Range2D range, float normalYXRatio) {
  float xLength = range.x().length();
  float yLength = range.y().length();
  float normalXLength = yLength > 0.f ? 0.5f * yLength / normalYXRatio : Range1D::k_defaultHalfLength;
  float normalYLength = (xLength > 0.f ? xLength : normalXLength) * normalYXRatio;
  return Range2D(sanitationHelper(range.x(), normalXLength), sanitationHelper(range.y(), normalYLength));
}

void Zoom::setFunction(FunctionEvaluation2DWithContext f, const void * model) {
  m_function = f;
  m_model = model;
  m_sampleUpToDate = false;
}

void Zoom::includePoint(Coordinate2D<float> p) {
  float x = p.x1();
  if (m_tMin <= x && x <= m_tMax) {
    m_range.extend(p);
  }
}

struct IntersectionParameters {
  Zoom::FunctionEvaluation2DWithContext f;
  const void * fModel;
  Zoom::FunctionEvaluation2DWithContext g;
  const void * gModel;
  Context * context;
};

static float evaluatorIntersection(float t, const void * aux) {
  const IntersectionParameters * params = static_cast<const IntersectionParameters *>(aux);
  Context * context = params->context;
  return params->f(t, params->fModel, context).x2() - params->g(t, params->gModel, context).x2();
}

void Zoom::fitIntersections(FunctionEvaluation2DWithContext otherFunction, const void * otherModel) {
  assert(std::isfinite(m_tMin) && std::isfinite(m_tMax));
  IntersectionParameters params = { .f = m_function, .fModel = m_model, .g = otherFunction, .gModel = otherModel, .context = m_context };
  float xCenter = 0.5f * (m_tMin + m_tMax);
  fitUsingSolver(xCenter, m_tMax, evaluatorIntersection, &params, Solver<float>::EvenOrOddRootInBracket);
  fitUsingSolver(xCenter, m_tMin, evaluatorIntersection, &params, Solver<float>::EvenOrOddRootInBracket);

  m_sampleUpToDate = false;
}

struct CallParameters {
  Zoom::FunctionEvaluation2DWithContext function;
  const void * model;
  Context * context;
};

static float evaluator(float t, const void * aux) {
  const CallParameters * p = static_cast<const CallParameters *>(aux);
  return p->function(t, p->model, p->context).x2();
}

void Zoom::fitX() {
  assert(std::isfinite(m_tMin) && std::isfinite(m_tMax));
  CallParameters params = { .function = m_function, .model = m_model, .context = m_context };
  /* Attempt to balance the range between m_tMin and m_tMax.
   * Step away from the center, as it is more likely to be a significant value
   * (typically zero).*/
  float xCenter = 0.5f * (m_tMin + m_tMax);
  float dx = std::max(Solver<float>::k_minimalAbsoluteStep, Solver<float>::k_relativePrecision * std::fabs(xCenter));
  fitUsingSolver(xCenter - dx, m_tMax, evaluator, &params, PointIsInteresting);
  fitUsingSolver(xCenter - dx, m_tMin, evaluator, &params, PointIsInteresting);

  /* TODO Add some margin around the X axis ? */

  /* At this point, range should be entirely determined by points of interest
   * and custom points defined by the user. */
  if (!m_range.x().isValid()) {
    m_range.x() = Range1D(-k_defaultHalfRange, k_defaultHalfRange);
  } else if (m_range.x().isEmpty()) {
    m_range.x() = Range1D(m_range.xMin() - k_defaultHalfRange, m_range.xMax() + k_defaultHalfRange);
  }

  m_sampleUpToDate = false;
}

void Zoom::fitOnlyY() {
  sampleY();

  /* Attempt to find a normalized range. This is not possible if the
   * interesting Y range is greater than the normalized Y range. */
  if (findNormalYAxis()) {
    return;
  }

  /* Attempt to compute a metric for the "order of magnitude" of the
   * expression, and fit Y around this metric. */
  if (findYAxisForOrderOfMagnitude()) {
    return;
  }

  /* FIXME Can this happen ? */
  float normalYLength = m_normalRatio * m_range.x().length();
  float halfYLength = 0.5f * normalYLength;
  m_range.y() = Range1D(-halfYLength, halfYLength);
}

void Zoom::fitBothXAndY(bool forceNormalization) {
  if (m_function && m_model) {
    sampleY();
    expandSparseWindow();
  }

  float currentRatio = m_range.ratio();
  bool closeToNormalized = m_normalRatio / k_orthonormalTolerance <= currentRatio && currentRatio <= m_normalRatio * k_orthonormalTolerance;
  if (forceNormalization || closeToNormalized) {
    m_range.setRatio(m_normalRatio, false);
  }
}

void Zoom::fitFullFunction() {
  assert(std::isfinite(m_tMin) && std::isfinite(m_tMax));
  float tStep = (m_tMax - m_tMin) / k_sampleSize;

  for (int i = 0; i < k_sampleSize; i++) {
    m_range.extend(approximate(m_tMin + i * tStep));
  }

  m_sampleUpToDate = false;
}

// Private

Solver<float>::Interest Zoom::PointIsInteresting(float ya, float yb, float yc) {
  Solver<float>::BracketTest tests[] = { Solver<float>::OddRootInBracket, Solver<float>::MinimumInBracket, Solver<float>::MaximumInBracket, Solver<float>::DiscontinuityInBracket };
  Solver<float>::Interest interest;
  for (Solver<float>::BracketTest & test : tests) {
    interest = test(ya, yb, yc);
    if (interest != Solver<float>::Interest::None) {
      break;
    }
  }
  return interest;
}

void Zoom::sampleY() {
  assert(m_sample);
  if (m_sampleUpToDate) {
    return;
  }
  assert(m_range.x().isValid() && !m_range.x().isEmpty());
  float xStep = m_range.x().length() / (k_sampleSize - 1);
  for (size_t i = 0; i < k_sampleSize; i++) {
    m_sample[i] = approximate(m_range.x().min() + i * xStep).x2();
  }
  m_sampleUpToDate = true;
}

void Zoom::fitUsingSolver(float xStart, float xEnd, Solver<float>::FunctionEvaluation eval, const void * aux, Solver<float>::BracketTest test) {
  Solver<float> solver(xStart, xEnd);
  Coordinate2D<float> p = solver.next(eval, aux, test, SelectFar);
  int n = 0;
  while (std::isfinite(p.x1()) && n < k_maxPointsOnOneSide) {
    n++;
    m_range.extend(p);
    p = solver.next(eval, aux, test, SelectFar);
  }
}

bool Zoom::findNormalYAxis() {
  assert(m_sampleUpToDate);

  /* For each value taken by the sample of the function on [xMin, xMax], given
   * a fixed value for yLength, we measure the number (referred to as breadth)
   * of other points that could be displayed if this value was chosen as yMin.
   * In other terms, given a sorted set Y={y1,...,yn} and a length dy, we look
   * for the pair 1<=i,j<=n such that :
   *   - yj - yi <= dy
   *   - i - j is maximal
   * The fact that the sample is sorted makes it possible to find i and j in
   * linear time.
   * In case of pairs having the same breadth, we chose the pair that minimizes
   * the criteria distanceFromCenter, which makes the window symmetrical when
   * dealing with linear functions. */

  float sortedSample[k_sampleSize];
  memcpy(sortedSample, m_sample, sizeof(m_sample));
  Helpers::Swap swap = [](int i, int j, void * ctx, int n) {
    float * sample = static_cast<float *>(ctx);
    float temp = sample[i];
    sample[i] = sample[j];
    sample[j] = temp;
  };
  Helpers::Compare compare = [](int i, int j, void * ctx, int n) {
    float * sample = static_cast<float *>(ctx);
    /* Put the NAN values at the beginning of the list */
    return sample[i] >= sample[j] || std::isnan(sample[j]);
  };
  Helpers::Sort(swap, compare, sortedSample, k_sampleSize);

  float yLength = m_normalRatio * m_range.x().length();

  int bestI = -1, bestBreadth = 0, bestDistance = INT_MAX;
  int j = 1;
  for (int i = 0; i < k_sampleSize; i++) {
    float yi = sortedSample[i];
    if (std::isnan(yi) || yi + yLength < m_range.y().max()) {
      /* Ignore NAN values, which are garanteed to be found at the beginning
       * of the list. Also make sure the new yMax value does not hide the Y
       * range set by interest and the user. */
      continue;
    }
    if (yi > m_range.y().min()) {
      /* Current yMin has been determined by interest or the user: we cannot
       * chose a yMin that would hide it. */
      break;
    }
    while (j < k_sampleSize && sortedSample[j] <= yi + yLength) {
      j++;
    }
    int breadth = j - i;
    int distanceFromCenter = std::abs<int>(j + i - k_sampleSize);
    if (breadth > bestBreadth || (breadth == bestBreadth && distanceFromCenter < bestDistance)) {
      bestI = i;
      bestBreadth = breadth;
      bestDistance = distanceFromCenter;
    }
  }
  if (bestI < 0) {
    return false;
  }
  int bestJ = bestI + bestBreadth - 1;
  assert(bestJ < k_sampleSize);

  /* Functions with a very steep slope might only take a small portion of the
   * X axis. Conversely, very flat functions may only take a small portion of
   * the Y range. In those cases, the ratio is not suitable. */
  constexpr float minimalXCoverage = 0.15f;
  bool doesNotCoverX = bestBreadth < minimalXCoverage * k_sampleSize;
  constexpr float minimalYCoverage = 0.3f;
  bool doesNotCoverY = sortedSample[bestJ] != sortedSample[bestI] && sortedSample[bestJ] - sortedSample[bestI] < minimalYCoverage * yLength;

  if (doesNotCoverX || doesNotCoverY) {
    return false;
  }

  float yCenter = (sortedSample[bestI] + sortedSample[bestJ]) * 0.5f;
  m_range.y() = Range1D(yCenter - yLength * 0.5f, yCenter + yLength * 0.5f);
  return true;
}

bool Zoom::findYAxisForOrderOfMagnitude() {
  assert(m_sampleUpToDate);

  /* We comute the log mean value of the expression, which gives an idea of the
   * order of magnitude of the function, to crop the Y axis. */
  constexpr float aboutZero = FLT_EPSILON;
  float sampleMin = FLT_MAX, sampleMax = -FLT_MAX;
  float nSum = 0.f, pSum = 0.f;
  int nPop = 0, pPop = 0;
  for (float y : m_sample) {
    sampleMin = std::min(sampleMin, y);
    sampleMax = std::max(sampleMax, y);
    float yAbs = std::fabs(y);
    if (yAbs <= aboutZero) {
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

  float yMin = nPop > 0 ? std::max(sampleMin, -std::exp(nSum / nPop + 1.f)) : sampleMin;
  float yMax = pPop > 0 ? std::min(sampleMax, std::exp(pSum / pPop + 1.f)) : sampleMax;
  if (std::isnan(yMin) || std::isnan(yMax)) {
    return false;
  }
  m_range.y() = Range1D(yMin, yMax);
  return true;
}

void Zoom::expandSparseWindow() {
  assert(m_sampleUpToDate);

  /* We compute the "empty center" of the window, i.e. the largest rectangle
   * (with same center and shape as the window) that does not contain any
   * point. If that rectangle is deemed too large, we consider that not enough
   * of the curve shows up on screen and we zoom out. */
  constexpr float emptyCenterMaxSize = 0.5f;
  constexpr float ratioCorrection = 4.f/3.f;

  Coordinate2D<float> center = m_range.center();
  float xLength = m_range.x().length();
  float yLength = m_range.y().length();
  float xStep = xLength / (k_sampleSize - 1);

  float emptyCenter = FLT_MAX;
  int n = 0;
  for (int i = 0; i < k_sampleSize; i++) {
    float y = m_sample[i];
    if (!std::isfinite(y)) {
      continue;
    }
    n++;
    float x = m_range.x().min() + i * xStep;
    /* r is the ratio between the window and the largest rectangle (with same
     * center and shape as the window) that does not contain (x,y).
     * i.e. the smallest zoom-in for which (x,y) is not visible. */
    float r = 2 * std::max(std::fabs(x - center.x1()) / xLength, std::fabs(y - center.x2()) / yLength);
    emptyCenter = std::min(r, emptyCenter);
  }

  if (emptyCenter > emptyCenterMaxSize && n > k_sampleSize / 10) {
    m_range.zoom(ratioCorrection + emptyCenter, center);
  }
}
#endif

}
