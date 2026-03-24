#include <omg/assertions.h>
#include <poincare/numeric_solver/solver.h>
#include <poincare/src/numeric_solver/solver_tests.h>

#include "hone_functions.h"

using namespace Poincare::Internal;

namespace Poincare::NumericSolver {

template <typename T>
Solver<T>::Solver(T xStart, T xEnd, FunctionEvaluation f, const void* aux,
                  BracketTest bracketTest, HoneResult honeFunction,
                  DiscontinuityEvaluation discontinuityTest)
    : m_growthSpeed(sizeof(T) == sizeof(double) ? GrowthSpeed::Precise
                                                : GrowthSpeed::Fast),
      m_evaluator(f),
      m_bracketTest(bracketTest),
      m_honeFunction(honeFunction),
      m_discontinuityTest(discontinuityTest),
      m_auxiliary(aux) {
  T searchStep = DefaultSearchStepForAmplitude(xEnd - xStart);
  reset(xStart, xEnd, searchStep);
};

Solver<double> BuildBasicRootSolver(
    double xStart, double xEnd, Solver<double>::FunctionEvaluation f,
    const void* aux,
    Solver<double>::DiscontinuityEvaluation discontinuityTest) {
  return Solver(xStart, xEnd, f, aux,
                SolverTests<double>::EvenOrOddRootInBracket,
                HoneFunctions::CompositeBrentForRoot, discontinuityTest);
}

template <typename T>
void Solver<T>::reset(T xStart, T xEnd, T searchStep) {
  m_xStart = xStart;
  m_xEnd = xEnd;
  setSearchStep(searchStep);
  m_solutionQueue = {};
}

template <typename T>
Interest Solver<T>::testBetween(Coordinate2D<T> a, Coordinate2D<T> b) {
  T x = (a.x() + b.x()) / 2.;
  Coordinate2D<T> middle(x, evaluateAt(x));
  return m_bracketTest(a, middle, b, m_auxiliary);
}

template <typename T>
Solution<T> Solver<T>::next() {
  // Check if there is a solution in the queue
  if (m_solutionQueue.size() > 0) {
    return registerSolution(m_solutionQueue.popFirst(), true);
  }

  Coordinate2D<T> p1, p2(m_xStart, evaluateAt(m_xStart)),
      p3(nextX(p2.x(), m_xEnd, static_cast<T>(1.)), k_NAN);
  p3.setY(evaluateAt(p3.x()));

  constexpr bool isDouble = sizeof(T) == sizeof(double);

  while ((m_xStart < p3.x()) == (p3.x() < m_xEnd)) {
    p1 = p2;
    p2 = p3;
    /* If the solver is in float, the slope is not used by minimalStep
     * so its computation is skipped here. */
    T slope =
        isDouble ? (p2.y() - p1.y()) / (p2.x() - p1.x()) : static_cast<T>(1.);
    p3.setX(nextX(p2.x(), m_xEnd, slope));
    p3.setY(evaluateAt(p3.x()));

    Coordinate2D<T> start = p1;
    Coordinate2D<T> middle = p2;
    Coordinate2D<T> end = p3;
    Interest interest = m_bracketTest(start, middle, end, m_auxiliary);
    /* If the solver is in float, we want it to be fast so the fine search
     * of interest around undefined intervals is skipped.
     * TODO: This is weird and should be specified another way */
    if (interest == Interest::None && isDouble &&
        isDiscontinuousBetween(start, end)) {
      /* If no interest was found and there is an undefined subinterval in the
       * interval, search for the largest interval without undef and then
       * recompute the interest in this interval. */
      Coordinate2D<T> a = start, b = middle, c = end;
      if (findMinimalIntervalContainingDiscontinuity(
              &a, &b, &c, MinimalStep(middle.x(), slope))) {
        // Try left of discontinuity
        interest = testBetween(start, a);
        if (interest != Interest::None) {
          end = a;
        } else {
          // Try right of discontinuity
          interest = testBetween(c, end);
          if (interest != Interest::None) {
            start = c;
          }
        }
      }
    }

    if (interest != Interest::None) {
      honeAndRoundSolution(start, end, interest);
      if (m_solutionQueue.size() > 0) {
        return registerSolution(m_solutionQueue.popFirst());
      }
    }
  }

  return k_noSolution<T>;
}

template <typename T>
void Solver<T>::stretch(bool onlyEnd) {
  T stepSign = m_xStart < m_xEnd ? static_cast<T>(1.) : static_cast<T>(-1.);
  if (!onlyEnd) {
    m_xStart -= m_searchStep * stepSign;
  }
  m_xEnd += m_searchStep * stepSign;
}

template <typename T>
T Solver<T>::DefaultSearchStepForAmplitude(T intervalAmplitude) {
  // NOTE: Is 100 too low ?
  constexpr T minimalNumberOfSteps = static_cast<T>(100.);
  return std::fabs(intervalAmplitude) / minimalNumberOfSteps;
}

template <typename T>
T Solver<T>::MinimalStep(T x, T slope) {
  T minimalStep = k_minimalPracticalStep;
  constexpr bool preventTooSmallStep = sizeof(T) == sizeof(double);
  if (preventTooSmallStep) {
    /* We make the minimal step dependent on the slope because if a function is
     * too flat, taking a step too small could lead to not detecting minimums
     * and maximums. Indeed, the function FunctionSeemsConstantOnTheInterval
     * would filter out mins and maxs.
     * We use e^7 because e^7 ~ 1000, so that if slope = 0,
     * minStep = 10e-6 * 10e3 = 0.001.
     * This is not applied to floats since the minStep is already big enough. */
    slope = std::fabs(slope);
    minimalStep =
        minimalStep * std::exp(static_cast<T>(7.) *
                               std::max(k_zero, static_cast<T>(1.) - slope));
  }
  return std::max(minimalStep, std::fabs(x) * k_relativePrecision);
}

template <typename T>
bool Solver<T>::validSolution(T x) const {
  T minStep = MinimalStep(m_xStart);
  /* NAN is implicitly handled by the comparisons. */
  return m_xStart < m_xEnd ? m_xStart + minStep < x && x < m_xEnd
                           : m_xEnd < x && x < m_xStart - minStep;
}

template <typename T>
T Solver<T>::nextX(T x, T direction, T slope) const {
  /* Compute the next step for the bracketing algorithm. The formula is derived
   * from the following criteria:
   * - using a fixed step would either lead to poor precision close to zero or
   *   prohibitive computation times on large intervals.
   * - we assume that for a typical function, distance between two points of
   *   interest is of the same magnitude as their abscissa ; as such, we want
   *   to sample with the same density on ]-10,10[ and ]-1000,1000[.
   * - we further assume that for a typical *high-school* function, points of
   *   interest are more likely to be close to the unit, rather than be
   *   around 1e6 or 1e-4.
   *
   * As such we use a formula of the form t+dt = t * φ(log|t|)
   * The geometric growth ensures we do not over-sample on large intervals,
   * and the term φ allows increasing the ratio in "less interesting" areas.
   *
   * As for the limits, we ensure that:
   * - even when |t| is large, dt never skips an order of magnitude
   *   i.e. 0.1 < |(t+dt)/t| < 10
   * - there is a minimal value for dt, to allow crossing zero.
   * - always sample a minimal number of points in the whole interval.
   *
   * NOTE: The growth speed as function of the magnitude of x has
   * an inflection point at:
   * (2/(3 * growthSpeedAcceleration)) ^ (1/3) + upperTypicalMagnitude
   * This is currently equal to 7.05, which matches the max magnitude
   * of x (10^8) */
  T baseGrowthSpeed = m_growthSpeed == GrowthSpeed::Precise
                          ? static_cast<T>(1.01)
                          : static_cast<T>(1.05);
  assert(baseGrowthSpeed > static_cast<T>(1.));
  constexpr T maximalGrowthSpeed = static_cast<T>(10.);
  constexpr T growthSpeedAcceleration = static_cast<T>(1e-2);
  /* Increase density between 0.1 and 100 */
  constexpr T lowerTypicalMagnitude = static_cast<T>(-1.);
  constexpr T upperTypicalMagnitude = static_cast<T>(3.);

  T maxStep = m_searchStep;
  T minStep = MinimalStep(x, slope);
  T stepSign = x < direction ? static_cast<T>(1.) : static_cast<T>(-1.);

  T magnitude = std::log10(std::fabs(x));
  if (!std::isfinite(magnitude) || minStep >= maxStep) {
    /* minStep can be greater than maxStep if we are operating on a very small
     * intervals of very large numbers */
    return x + stepSign * minStep;
  }
  /* We define a piecewise function φ such that:
   * - φ is constant on [-1,2] and return baseGrowthSpeed, making t progress
   *   geometrically from 0.1 to 100.
   * - φ varies continuously from baseGrowthSpeed, to maximalGrowthSpeed at ∞.
   * The particular shape used there (an exponential of the negative cubed
   * magnitude) provides a smooth transition up until log|t|~±8. */
  T ratio;
  if (lowerTypicalMagnitude <= magnitude &&
      magnitude <= upperTypicalMagnitude) {
    ratio = baseGrowthSpeed;
  } else {
    T magnitudeDelta = magnitude < lowerTypicalMagnitude
                           ? lowerTypicalMagnitude - magnitude
                           : magnitude - upperTypicalMagnitude;
    assert(magnitudeDelta > 0);
    ratio = maximalGrowthSpeed -
            (maximalGrowthSpeed - baseGrowthSpeed) *
                std::exp(growthSpeedAcceleration *
                         -std::pow(magnitudeDelta, static_cast<T>(3.)));
  }
  /* If the next step is toward zero, divide the postion, otherwise multiply. */
  assert(ratio > static_cast<T>(1.));
  T x2 = (x < direction) == (x < k_zero) ? x / ratio : x * ratio;
  if (std::fabs(x - x2) > maxStep) {
    x2 = x + stepSign * maxStep;
  }
  if (std::fabs(x - x2) < minStep) {
    x2 = x + stepSign * minStep;
  }
  assert((x < x2) == (x < direction));
  assert(x2 != x);
  return x2;
}

template <typename T>
T Solver<T>::MagicRound(T x) {
  constexpr T k_roundingOrder = 2. * k_minimalPracticalStep;  // Magic number
  return k_roundingOrder * std::round(x / k_roundingOrder);
}

template <typename T>
void Solver<T>::honeAndRoundSolution(Coordinate2D<T> start, Coordinate2D<T> end,
                                     Interest interest) {
  assert(m_solutionQueue.size() == 0);
  // Handled in DiscontinuitySolver
  assert(interest != Interest::ReachedDiscontinuity);

  bool isDiscontinuous = isDiscontinuousBetween(start, end);

  /* WARNING: This is a hack for discontinuous functions. BrentForRoot
   * needs to be very precise to find a root that is on the discontinuity bound.
   *
   * For other functions, we prefer a precision of NullTolerance because if the
   * precision is too high, the honing is more subject to approximations errors.
   */
  constexpr T precisionForDiscontinuousFunctions =
      k_relativePrecision * k_minimalAbsoluteStep;
  T xPrecision = isDiscontinuous ? precisionForDiscontinuousFunctions
                                 : NullTolerance(start.x());
  Coordinate2D<T> xy = m_honeFunction(m_evaluator, m_auxiliary, start.x(),
                                      end.x(), interest, xPrecision);

  /* If the function is discontinuous and the discontinuity is on both sides
   * of the abscissa axis, a fake root could have been found. Filter it out
   * using null tolerance. This can happens for example with the functions:
   *   > f(x) = floor(x) - 0.5 for x == 1
   *   > f(x) = x / abs(x) for x == 0 */
  if (isDiscontinuous && interest == Interest::Root &&
      std::fabs(xy.y()) > NullTolerance(xy.x())) {
    return;
  }

  if (!std::isfinite(xy.x()) || !validSolution(xy.x())) {
    return;
  }

  T x = xy.x();
  /* When searching for an extremum, the function can take the extremum value
   * on several abscissas, and Brent can pick up any of them. This deviation
   * is particularly visible if the theoretical solution is an integer. */
  T roundX = MagicRound(x);
  T fRoundX = evaluateAt(roundX);
  if (std::isnan(fRoundX) && interest != Interest::Discontinuity) {
    // We might have a discontinuity, the solution might not be valid
    return;
  }
  // f(x) is different from the honed solution when searching intersections
  T fx = evaluateAt(x);
  if (std::isfinite(roundX) && validSolution(roundX)) {
    /* Filter out solutions that are close to a discontinuity. This can
     * happen with functions such as  y = (-x when x < 0, x-1 otherwise)
     * with which a root is found in (0,0) when it should not.
     * It is assumed that the piecewise condition are more often than not
     * integers or of a magnitude closer to roundX than x. So if the
     * condition of roundX is different from x, this means that the solution
     * found is probably on an open interval. */
    if (isDiscontinuousBetween(Coordinate2D<T>(x, fx),
                               Coordinate2D<T>(roundX, fRoundX))) {
      return;
    }
    if (fRoundX == fx ||
        (interest == Interest::Root && std::fabs(fRoundX) < std::fabs(fx)) ||
        (interest == Interest::Minimum && fRoundX < fx) ||
        (interest == Interest::Maximum && fRoundX > fx)) {
      // Round is better
      xy.setX(roundX);
      if (xy.y() == fx) {
        xy.setY(fRoundX);
      }
    }
  }
  assert(std::isfinite(xy.x()));
  if (std::isfinite(xy.y()) || interest == Interest::Discontinuity) {
    m_solutionQueue.push(CoordToSolution(xy, interest));
  }
}

template <typename T>
Solution<T> Solver<T>::registerSolution(Solution<T> solution, bool wasQueued) {
  if (std::isnan(solution.x)) {
    return k_noSolution<T>;
  }
  assert((wasQueued && solution.x == m_xStart) ||
         (!wasQueued && validSolution(solution.x)));
  if (std::fabs(solution.y) < NullTolerance(solution.x)) {
    solution.y = k_zero;
  }
  m_xStart = solution.x;
  assert((solution.interest == Interest::None) == std::isnan(solution.x));
  return solution;
}

template <typename T>
bool Solver<T>::findMinimalIntervalContainingDiscontinuity(
    Coordinate2D<T>* start, Coordinate2D<T>* middle, Coordinate2D<T>* end,
    T minimalSizeOfInterval) const {
  COSTLY_ASSERT(isDiscontinuousBetween(*start, *end));

  // Initialize middle if needed
  if (std::isnan(middle->x())) {
    middle->setX((start->x() + end->x()) / 2.0);
    middle->setY(evaluateAt(middle->x()));
  }

  while (std::fabs(end->x() - start->x()) >= minimalSizeOfInterval) {
    bool firstAreaIsDiscontinuous = isDiscontinuousBetween(*start, *middle);
    bool secondAreaIsDiscontinuous = isDiscontinuousBetween(*middle, *end);
    if (firstAreaIsDiscontinuous == secondAreaIsDiscontinuous) {
      /* Either too many discontinuities and/or step is too big
       * Or couldn't find any discontinuities */
      return false;
    }
    if (firstAreaIsDiscontinuous) {
      if (std::fabs(end->x() - middle->x()) == 0) {
        // start and end are too big and end-start is too small
        return false;
      }
      *end = *middle;
    } else {
      if (std::fabs(start->x() - middle->x()) == 0) {
        // start and end are too big and end-start is too small
        return false;
      }
      assert(secondAreaIsDiscontinuous);
      *start = *middle;
    }
    middle->setX((start->x() + end->x()) / 2.0);
    middle->setY(evaluateAt(middle->x()));
    COSTLY_ASSERT(isDiscontinuousBetween(*start, *end));
  }
  COSTLY_ASSERT(std::fabs(end->x() - start->x()) > 0 &&
                std::fabs(end->x() - start->x()) <= minimalSizeOfInterval &&
                isDiscontinuousBetween(*start, *end));
  return true;
}

template class Solver<double>;
template class Solver<float>;

}  // namespace Poincare::NumericSolver
