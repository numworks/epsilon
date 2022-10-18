#include <poincare/zoom.h>
#include <string.h>

namespace Poincare {

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

void Zoom::fitX() {
  assert(std::isfinite(m_tMin) && std::isfinite(m_tMax));
  /* Attempt to balance the range between m_tMin and m_tMax. */
  float xCenter = 0.5f * (m_tMin + m_tMax);
  grossFitToInterest(xCenter, m_tMax);
  grossFitToInterest(xCenter, m_tMin);

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

Coordinate2D<float> Zoom::SelectMiddle(Solver<float>::FunctionEvaluation f, const void * model, float a, float b, Solver<float>::Interest, float precision) {
  float c = 0.5f * (a + b);
  return Coordinate2D<float>(c, f(c, model));
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

struct CallParameters {
  Zoom::FunctionEvaluation2DWithContext function;
  const void * model;
  Context * context;
};

static float evaluator(float t, const void * aux) {
  const CallParameters * p = static_cast<const CallParameters *>(aux);
  return p->function(t, p->model, p->context).x2();
}

void Zoom::grossFitToInterest(float xStart, float xEnd) {
  CallParameters params = { .function = m_function, .model = m_model, .context = m_context };

  Solver<float> solver(xStart, xEnd);
  Coordinate2D<float> p = solver.next(evaluator, &params, PointIsInteresting, SelectMiddle);
  int n = 0;
  while (std::isfinite(p.x1()) && n < k_maxPointsOnOneSide) {
    n++;
    m_range.extend(p);
    p = solver.next(evaluator, &params, PointIsInteresting, SelectMiddle);
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
    while (sortedSample[j] <= yi + yLength) {
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

  /* Functions with a very steep slope might only take a small portion of the
   * X axis. Conversely, very flat functions may only take a small portion of
   * the Y range. In those cases, the ratio is not suitable. */
  constexpr float minimalXCoverage = 0.15f;
  constexpr float minimalYCoverage = 0.3f;
  if (bestBreadth < minimalXCoverage * k_sampleSize || sortedSample[bestJ] - sortedSample[bestI] < minimalYCoverage * yLength) {
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

}
