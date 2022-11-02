#include <poincare/zoom.h>
#include <poincare/trinary_boolean.h>
#include <string.h>

namespace Poincare {

// HorizontalAsymptoteHelper

void Zoom::HorizontalAsymptoteHelper::update(Coordinate2D<float> p, float slope) {
  Coordinate2D<float> * bound = p.x1() < m_center ? &m_left : &m_right;
  slope = std::fabs(slope);
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

  assert(!const_cast<Zoom *>(this)->m_interestingRange.x().isValid() || (result.xMin() <= m_interestingRange.xMin() && m_interestingRange.xMax() <= result.xMax()));
  assert(!const_cast<Zoom *>(this)->m_interestingRange.y().isValid() || (result.yMin() <= m_interestingRange.yMin() && m_interestingRange.yMax() <= result.yMax()));
  assert(result.x().isValid() && result.y().isValid() && !result.x().isEmpty() && !result.y().isEmpty());
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
  HorizontalAsymptoteHelper asymptotes(m_bounds.center());
  InterestParameters params = { .f = f, .model = model, .context = m_context, .asymptotes = &asymptotes };
  Solver<float>::FunctionEvaluation evaluator = [](float t, const void * aux) {
    const InterestParameters * p = static_cast<const InterestParameters *>(aux);
    return p->f(t, p->model, p->context).x2(); // TODO Zoom could also work for x=f(y) functions
  };
  bool leftInterrupted, rightInterrupted;
  fitWithSolver(&leftInterrupted, &rightInterrupted, evaluator, &params, PointIsInteresting, HonePoint);
  /* If the search has been interrupted, the curve is supposed to have an
   * infinite number of points in this direction. An horizontal asymptote
   * would be the result of a sampling artifact and can be discarded. */
  if (!leftInterrupted) {
    m_interestingRange.extend(asymptotes.left());
  }
  if (!rightInterrupted) {
    m_interestingRange.extend(asymptotes.right());
  }
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
  Solver<float>::HoneResult hone = [](Solver<float>::FunctionEvaluation f, const void * aux, float, float b, Solver<float>::Interest, float) {
    const Parameters * p = static_cast<const Parameters *>(aux);
    /* Return the faraway point (i.e. b) to avoid finding the same intersection
     * twice. */
    return p->f1(b, p->model1, p->context);
  };
  bool dummy;
  fitWithSolver(&dummy, &dummy, evaluator, &params, Solver<float>::EvenOrOddRootInBracket, hone);
}

void Zoom::fitMagnitude(Function2DWithContext f, const void * model) {
  /* We compute the log mean value of the expression, which gives an idea of the
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
  float yMax = pPop > 0 ? std::min(sample.max(), std::exp(pSum / pPop  + 1.f)) : sample.max();
  m_magnitudeYRange.extend(yMax);
  float yMin = nPop > 0 ? std::max(sample.min(), -std::exp(nSum / nPop  + 1.f)) : sample.min();
  m_magnitudeYRange.extend(yMin);
}

// Zoom - Private

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

Solver<float>::Interest Zoom::PointIsInteresting(Coordinate2D<float> a, Coordinate2D<float> b, Coordinate2D<float> c, const void * aux) {
  const InterestParameters * params = static_cast<const InterestParameters *>(aux);
  float slope = (c.x2() - a.x2()) / (c.x1() - a.x1());
  params->asymptotes->update(c, slope);
  return pointIsInterestingHelper(a, b, c, aux);
}

Coordinate2D<float> Zoom::HonePoint(Solver<float>::FunctionEvaluation f, const void * aux, float a, float b, Solver<float>::Interest interest, float precision) {
  /* Use a simple dichotomy in [a,b] to hone in on the point of interest
   * without using the costly Brent methods. */
  constexpr int k_numberOfIterations = 10; // TODO Tune
  bool continuous;
  /* Define three points m, u and v such that a < u < m < v < b. Then, we can
   * determine wether the point of interest exists on [a,m] or [m,b]. */
  float m = 0.5f * (a + b);
  Coordinate2D<float> pa(a, f(a, aux)), pb(b, f(b, aux)), pm(m, f(m, aux));
  Coordinate2D<float> pu, pv;

  for (int i = 0; i < k_numberOfIterations; i++) {
    pu.setX1(0.5f * (pa.x1() + pm.x1()));
    pu.setX2(f(pu.x1(), aux));
    pv.setX1(0.5f * (pm.x1() + pb.x1()));
    pv.setX2(f(pv.x1(), aux));

    if (i == k_numberOfIterations - 1) {
      /* A continuous function should not diverge as we get closer to its
       * extremum, meaning the ordinates of the points should get closer to
       * one another. */
      continuous = std::fabs(pu.x2() - pm.x2()) <= std::fabs(pa.x2() - pm.x2()) && std::fabs(pv.x2() - pm.x2()) <= std::fabs(pb.x2() - pm.x2());
    }

    /* Select the interval that contains the point of interest. If, because of
     * some artefacts, both or neither contains a point, we favor the interval
     * on the far side (i.e. [m,b]) to avoid finding the same point twice. */
    if (pointIsInterestingHelper(pm, pv, pb, aux) != Solver<float>::Interest::None || pointIsInterestingHelper(pa, pu, pm, aux) == Solver<float>::Interest::None) {
      pa = pm;
      pm = pv;
    } else {
      pb = pm;
      pm = pu;
    }
  }
  /* If the function is discontinuous around the solution (e.g. 1/x^2), we
   * discard the y value to avoid zooming in on diverging points. */
  return Coordinate2D<float>(pb.x1(), interest == Solver<float>::Interest::Root ? 0.f : continuous ? pb.x2() : NAN);
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
  /* yCanBeNormalized is true if:
   * - a normalized Y range can fit the interesting Y range. We only count the
   *   interesting Y range for this part as discarding the part that comes from
   *   the magnitude is not an issue.
   * - the Y range (interesting + magnitude) makes up for at least 30% of the normalized Y
   *   range (i.e. the curve does not appear squeezed). */
  bool yCanBeNormalized = yLengthNormalized * k_minimalYCoverage <= yLength && thisRange.y().length() <= yLengthNormalized;
  if (!yRange.isValid() || yLength == 0.f || yCanBeNormalized) {
    float yCenter = yRange.center();
    if (!std::isfinite(yCenter)) {
      yCenter = 0.f;
    }
    if (yCenter - 0.5f * yLengthNormalized > thisRange.yMin()) {
      yRange = Range1D(thisRange.yMin(), thisRange.yMin() + yLengthNormalized);
    } else if (yCenter + 0.5f * yLengthNormalized < thisRange.yMax()) {
      yRange = Range1D(thisRange.yMax() - yLengthNormalized, thisRange.yMax());
    } else {
      yRange = Range1D(yCenter - 0.5f * yLengthNormalized, yCenter + 0.5f * yLengthNormalized);
    }
  } else if (xLengthNormalized * k_minimalXCoverage <= xLength && xLength <= xLengthNormalized) {
    float xCenter = xRange.center();
    xRange = Range1D(xCenter - xLengthNormalized * 0.5f, xCenter + xLengthNormalized * 0.5f);
  }

  assert(xRange.isValid() && !xRange.isEmpty() && yRange.isValid() && !yRange.isEmpty());
  return Range2D(xRange, yRange);
}

void Zoom::fitWithSolver(bool * leftInterrupted, bool * rightInterrupted, Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test, Solver<float>::HoneResult hone) {
  assert(leftInterrupted && rightInterrupted);

  /* Pick margin large enough to detect an extremum around zero, for some
   * particularly flat function such as (x+10)(x-10). */
  constexpr float k_marginAroundZero = 1e-2f;

  float c = m_bounds.center();
  float d = std::max(k_marginAroundZero, std::fabs(c * Solver<float>::k_relativePrecision));
  *rightInterrupted = fitWithSolverHelper(c + d, m_bounds.max(), evaluator, aux, test, hone);
  *leftInterrupted = fitWithSolverHelper(c - d, m_bounds.min(), evaluator, aux, test, hone);

  Coordinate2D<float> p1(c - d, evaluator(c - d, aux));
  Coordinate2D<float> p2(c, evaluator(c, aux));
  Coordinate2D<float> p3(c + d, evaluator(c + d, aux));
  if (pointIsInterestingHelper(p1, p2, p3, aux) != Solver<float>::Interest::None) {
    m_interestingRange.extend(p2);
  }
}

bool Zoom::fitWithSolverHelper(float start, float end, Solver<float>::FunctionEvaluation evaluator, const void * aux, Solver<float>::BracketTest test, Solver<float>::HoneResult hone) {
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
  Coordinate2D<float> p = solver.next(evaluator, aux, test, hone);
  int n = 0;
  while (solver.lastInterest() != Solver<float>::Interest::None) {
    m_interestingRange.extend(p);
    p = solver.next(evaluator, aux, test, hone);
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

}
