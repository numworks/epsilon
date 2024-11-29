#include <poincare/numeric/solver.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/continuity.h>
#include <poincare/src/expression/dependency.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/memory/pattern_matching.h>

#include "solver_algorithms.h"

using namespace Poincare::Internal;

namespace Poincare {

/* Using our consteval operator- inside a template<float> does not work with
 * llvm14 it works with 17. */
constexpr KTree minusOne = -1_e;

template <typename T>
Solver<T>::Solver(T xStart, T xEnd, Context* context)
    : m_xStart(xStart),
      m_xEnd(xEnd),
      m_context(context),
      m_growthSpeed(sizeof(T) == sizeof(double) ? GrowthSpeed::Precise
                                                : GrowthSpeed::Fast) {
  setSearchStep(DefaultSearchStepForAmplitude(xEnd - xStart));
}

template <typename T>
typename Solver<T>::Solution Solver<T>::next(
    FunctionEvaluation f, const void* aux, BracketTest test, HoneResult hone,
    DiscontinuityEvaluation discontinuityTest) {
  // Check if there is a solution in the queue
  if (!m_solutionQueue.isEmpty()) {
    return registerSolution(m_solutionQueue.pop(), true);
  }

  Coordinate2D<T> p1, p2(m_xStart, f(m_xStart, aux)),
      p3(nextX(p2.x(), m_xEnd, static_cast<T>(1.)), k_NAN);
  p3.setY(f(p3.x(), aux));

  constexpr bool isDouble = sizeof(T) == sizeof(double);

  while ((m_xStart < p3.x()) == (p3.x() < m_xEnd)) {
    p1 = p2;
    p2 = p3;
    /* If the solver is in float, the slope is not used by minimalStep
     * so its computation is skipped here. */
    T slope =
        isDouble ? (p2.y() - p1.y()) / (p2.x() - p1.x()) : static_cast<T>(1.);
    p3.setX(nextX(p2.x(), m_xEnd, slope));
    p3.setY(f(p3.x(), aux));

    Coordinate2D<T> start = p1;
    Coordinate2D<T> middle = p2;
    Coordinate2D<T> end = p3;
    Interest interest = test(start, middle, end, aux);
    /* If the solver is in float, we want it to be fast so the fine search
     * of interest around undefined intervals is skipped. */
    if (interest == Interest::None && isDouble &&
        UndefinedInBracket(start, middle, end, aux) ==
            Interest::Discontinuity) {
      /* If no interest was found and there is an undefined subinterval in the
       * interval, search for the largest interval without undef and then
       * recompute the interest in this interval. */
      ExcludeUndefinedFromBracket(&start, &middle, &end, f, aux,
                                  MinimalStep(middle.x(), slope));
      interest = test(start, middle, end, aux);
    }

    if (interest != Interest::None) {
      honeAndRoundSolution(f, aux, start, end, interest, hone,
                           discontinuityTest);
      if (!m_solutionQueue.isEmpty()) {
        return registerSolution(m_solutionQueue.pop());
      }
    }
  }

  return Solution();
}

template <typename T>
typename Solver<T>::Solution Solver<T>::next(const Tree* e, BracketTest test,
                                             HoneResult hone) {
  if (e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isRandomized(); })) {
    return Solution();
  }
  /* TODO_PCJ: Either Pass ApproximationContext(m_context, m_complexFormat,
   * m_angleUnit) or ensure expression is projected. */
  FunctionEvaluation f = [](T x, const void* aux) {
    const Internal::Tree* e = reinterpret_cast<const Internal::Tree*>(aux);
    return Approximation::To<T>(
        e, x, Approximation::Parameters{.isRootAndCanHaveRandom = true});
  };

  return next(f, e, test, hone, &DiscontinuityTestBetweenPoints);
}

template <typename T>
typename Solver<T>::Solution Solver<T>::nextRoot(const Tree* e) {
  if (e->hasDescendantSatisfying(
          [](const Tree* e) { return e->isRandomized(); })) {
    return Solution();
  }

  switch (e->type()) {
    case Type::Mult:
      /* x*y = 0 => x = 0 or y = 0 */
      return registerRoot(nextRootInMultiplication(e));

    case Type::Add:
    case Type::Sub:
      return registerRoot(nextRootInAddition(e));

    case Type::Pow:
    case Type::Root:
    case Type::Div:
      /* f(x,y) = 0 => x = 0 */
      return registerRoot(nextPossibleRootInChild(e, 0));

    case Type::Abs:
    case Type::ATan:
    case Type::SinH:
    case Type::Opposite:
    case Type::Sqrt:
      /* f(x) = 0 <=> x = 0 */
      return nextRoot(e->child(0));

    case Type::Dep:
      return registerRoot(nextRootInDependency(e));

    default:
      if (!GetComplexSign(e).canBeNull()) {
        return Solution();
      }

      Solution res = next(e, EvenOrOddRootInBracket, CompositeBrentForRoot);
      if (res.interest() != Interest::None) {
        res.setInterest(Interest::Root);
      }
      return res;
  }
}

template <typename T>
typename Solver<T>::Solution Solver<T>::nextMinimum(const Tree* e) {
  /* TODO We could add a layer of formal resolution:
   * - use the derivative (could be an optional argument to avoid recomputing
   *   it every time)
   * - since d(f°g) = dg×df°g, if f is known to be monotonous (i.e. df≠0), look
   *   for the extrema of g. */
  return next(e, MinimumInBracket, SafeBrentMinimum);
}

template <typename T>
typename Solver<T>::Solution Solver<T>::nextIntersection(
    const Tree* e1, const Tree* e2, const Tree** memoizedDifference) {
  if (!memoizedDifference) {
    Tree* diff = nullptr;
    Solution result = nextIntersection(e1, e2, const_cast<const Tree**>(&diff));
    diff->removeTree();
    return result;
  }
  assert(memoizedDifference);
  if (*memoizedDifference == nullptr) {
    /* TODO simplify with if
     * we decide that functions should be simplified
     * Either Pass ReductionContext(m_context, m_complexFormat,
     * m_angleUnit, UnitFormat::Metric, ReductionTarget::SystemForAnalysis) or
     * ensure expression is projected. */
    *memoizedDifference = PatternMatching::Create(KAdd(KA, KMult(minusOne, KB)),
                                                  {.KA = e1, .KB = e2});
  }
  Solution root = nextRoot(*memoizedDifference);
  if (root.interest() != Interest::Root) {
    assert(root.interest() == Interest::None);
    return Solution();
  }
  T x = root.x();
  T y1 = Approximation::To<T>(
      e1, x, Approximation::Parameters{.isRootAndCanHaveRandom = true});
  T y2 = Approximation::To<T>(
      e2, x, Approximation::Parameters{.isRootAndCanHaveRandom = true});
  if (!std::isfinite(y1) || !std::isfinite(y2)) {
    /* Sometimes, with expressions e1 and e2 that take extreme values like x^x
     * or undef expressions in specific points like x^2/x, the root of the
     * difference yields an infinite or a nan value when e1 or e2 is
     * evaluated. It means the intersection was incorrectly computed, and the
     * search continues. */
    return nextIntersection(e1, e2, memoizedDifference);
  }
  /* Result is not always exactly the same due to approximation errors. Take
   * the middle of the two values. */
  return Solution(x, (y1 + y2) / 2., Interest::Intersection);
}

template <typename T>
void Solver<T>::stretch() {
  T stepSign = m_xStart < m_xEnd ? static_cast<T>(1.) : static_cast<T>(-1.);
  m_xStart -= m_searchStep * stepSign;
  m_xEnd += m_searchStep * stepSign;
}

template <typename T>
typename Solver<T>::Interest Solver<T>::EvenOrOddRootInBracket(
    Coordinate2D<T> a, Coordinate2D<T> b, Coordinate2D<T> c, const void* aux) {
  Interest root = OddRootInBracket(a, b, c, aux);
  if (root != Interest::None) {
    return root;
  }
  /* FIXME Check the sign of a,b and c. A minimum can only be a root if b is
   * positive. */
  Interest extremum = MinimumInBracket(a, b, c, aux);
  return extremum == Interest::None ? MaximumInBracket(a, b, c, aux) : extremum;
}

template <typename T>
Coordinate2D<T> Solver<T>::SafeBrentMinimum(FunctionEvaluation f,
                                            const void* aux, T xMin, T xMax,
                                            Interest interest, T xPrecision,
                                            OMG::Troolean discontinuous) {
  if (xMax < xMin) {
    return SafeBrentMinimum(f, aux, xMax, xMin, interest, xPrecision,
                            discontinuous);
  }
  assert(xMin < xMax);

  if (FunctionSeemsConstantOnTheInterval(f, aux, xMin, xMax)) {
    /* Some fake minimums can be detected due to approximations errors like in
     * f(x) = x/abs(x) in complex mode. */
    return Coordinate2D<T>();
  }

  return SolverAlgorithms::BrentMinimum(f, aux, xMin, xMax, interest,
                                        xPrecision);
}

template <typename T>
Coordinate2D<T> Solver<T>::SafeBrentMaximum(FunctionEvaluation f,
                                            const void* aux, T xMin, T xMax,
                                            Interest interest, T xPrecision,
                                            OMG::Troolean discontinuous) {
  const void* pack[] = {&f, aux};
  FunctionEvaluation minusF = [](T x, const void* aux) {
    const void* const* param = reinterpret_cast<const void* const*>(aux);
    const FunctionEvaluation* f =
        reinterpret_cast<const FunctionEvaluation*>(param[0]);
    return -(*f)(x, param[1]);
  };
  Coordinate2D<T> res = SafeBrentMinimum(minusF, pack, xMin, xMax, interest,
                                         xPrecision, discontinuous);
  return Coordinate2D<T>(res.x(), -res.y());
}

template <typename T>
Coordinate2D<T> Solver<T>::CompositeBrentForRoot(FunctionEvaluation f,
                                                 const void* aux, T xMin,
                                                 T xMax, Interest interest,
                                                 T xPrecision,
                                                 OMG::Troolean discontinuous) {
  if (interest == Interest::Root) {
    Coordinate2D<T> xy =
        SolverAlgorithms::BrentRoot(f, aux, xMin, xMax, interest, xPrecision);
    /* If the function is discontinuous and the discontinuity is on both sides
     * of the abscissa axis, a fake root could have been found. Filter it out
     * using null tolerance. This can happens for example with the functions:
     *   > f(x) = floor(x) - 0.5 for x == 1
     *   > f(x) = x / abs(x) for x == 0 */
    if (discontinuous == OMG::Troolean::True &&
        std::fabs(xy.y()) > NullTolerance(xy.x())) {
      return Coordinate2D<T>();
    }
    return xy;
  }
  Coordinate2D<T> res;
  if (interest == Interest::LocalMinimum) {
    res = SafeBrentMinimum(f, aux, xMin, xMax, interest, xPrecision,
                           discontinuous);
  } else {
    assert(interest == Interest::LocalMaximum);
    res = SafeBrentMaximum(f, aux, xMin, xMax, interest, xPrecision,
                           discontinuous);
  }
  return std::isfinite(res.x()) && std::fabs(res.y()) < NullTolerance(res.x())
             ? res
             : Coordinate2D<T>();
}

template <typename T>
bool Solver<T>::DiscontinuityTestBetweenPoints(Coordinate2D<T> a,
                                               Coordinate2D<T> b,
                                               const void* aux) {
  const Internal::Tree* e = reinterpret_cast<const Internal::Tree*>(aux);
  return Continuity::IsDiscontinuousBetweenValues<T>(e, a.x(), b.x());
};

template <typename T>
Coordinate2D<T> Solver<T>::FindUndefinedIntervalBound(
    Coordinate2D<T> p1, Coordinate2D<T> p2, Coordinate2D<T> p3,
    FunctionEvaluation f, const void* aux, T minimalSizeOfInterval,
    bool findStart) {
  /* Search for the smallest interval that contains the undefined and
   * return the largest interval that does not intersect with it. */
  Coordinate2D<T> lowerBoundOfDiscontinuity = p1;
  Coordinate2D<T> middleOfDiscontinuity = p2;
  Coordinate2D<T> upperBoundOfDiscontinuity = p3;
  Coordinate2D<T>* undefinedBound =
      findStart ? &upperBoundOfDiscontinuity : &lowerBoundOfDiscontinuity;
  Coordinate2D<T>* definedBound =
      findStart ? &lowerBoundOfDiscontinuity : &upperBoundOfDiscontinuity;
  assert(std::isfinite(definedBound->y()) && std::isnan(undefinedBound->y()));
  while (upperBoundOfDiscontinuity.x() - lowerBoundOfDiscontinuity.x() >=
         minimalSizeOfInterval) {
    if (std::isnan(middleOfDiscontinuity.y())) {
      *undefinedBound = middleOfDiscontinuity;
    } else if (std::isfinite(middleOfDiscontinuity.y())) {
      *definedBound = middleOfDiscontinuity;
    } else {
      assert(std::isinf(middleOfDiscontinuity.y()));
      return Coordinate2D<T>();
    }
    middleOfDiscontinuity.setX(
        (lowerBoundOfDiscontinuity.x() + upperBoundOfDiscontinuity.x()) / 2.0);
    middleOfDiscontinuity.setY(f(middleOfDiscontinuity.x(), aux));
    assert(std::isfinite(definedBound->y()) && std::isnan(undefinedBound->y()));
  }
  return *definedBound;
}

template <typename T>
void Solver<T>::ExcludeUndefinedFromBracket(
    Coordinate2D<T>* p1, Coordinate2D<T>* p2, Coordinate2D<T>* p3,
    FunctionEvaluation f, const void* aux, T minimalSizeOfInterval) {
  assert(UndefinedInBracket(*p1, *p2, *p3, aux) == Interest::Discontinuity);
  assert(std::isnan(p1->y()) || std::isnan(p3->y()));
  // Find the smallest interval containing the undefined
  bool cropEnd = std::isnan(p3->y());
  Coordinate2D<T> newBound = FindUndefinedIntervalBound(
      *p1, *p2, *p3, f, aux, minimalSizeOfInterval, cropEnd);
  // Set p1, p2 and p3 outside of the interval containing the undefined
  if (std::isfinite(newBound.x())) {
    (cropEnd ? *p3 : *p1) = newBound;
    p2->setX((p1->x() + p3->x()) / 2.0);
    p2->setY(f(p2->x(), aux));
  }
}

template <typename T>
bool Solver<T>::FunctionSeemsConstantOnTheInterval(
    Solver<T>::FunctionEvaluation f, const void* aux, T xMin, T xMax) {
  assert(xMin < xMax);
  constexpr int k_numberOfSteps = 20;
  T values[k_numberOfSteps];
  int valuesCount[k_numberOfSteps];
  int currentNumberOfValues = 0;
  /* This loop computes 20 values of f on the interval and then checks the
   * repartition of these values. If the function takes a few number of
   * different values, it might mean that f is a constant function but
   * approximation errors led to thinking there was a minimum in the interval.
   * To measure this "repartition" of values, the entropy of the data is
   * then calculated. */
  for (int i = 0; i < k_numberOfSteps; i++) {
    T currentValue =
        f(xMin + (static_cast<T>(i) / k_numberOfSteps) * (xMax - xMin), aux);
    bool addValueToArray = true;
    for (int k = 0; k < currentNumberOfValues; k++) {
      if (values[k] == currentValue) {
        addValueToArray = false;
        valuesCount[k]++;
        break;
      }
    }
    if (addValueToArray) {
      values[currentNumberOfValues] = currentValue;
      valuesCount[currentNumberOfValues] = 1;
      currentNumberOfValues++;
    }
  }

  /* Entropy = -sum(log(pk)*pk) where pk is the probability of taking the
   * k-th value. */
  T entropy = 0.;
  for (int k = 0; k < currentNumberOfValues; k++) {
    T probabilityOfValue =
        static_cast<T>(valuesCount[k]) / (k_numberOfSteps + 1);
    entropy += -std::log(probabilityOfValue) * probabilityOfValue;
  }

  // maxEntropy = ln(k_numberOfSteps), unfortunately std::log is not constexpr
  constexpr T k_maxEntropy = static_cast<T>(2.995733);
  // Check that k_maxEntropy ~ ln(k_numberOfSteps)
  assert(std::log(static_cast<T>(k_numberOfSteps)) <= k_maxEntropy &&
         std::log(static_cast<T>(k_numberOfSteps)) >
             k_maxEntropy - static_cast<T>(1E-6));
  assert(entropy >= 0 && entropy <= k_maxEntropy);
  /* If the entropy of the data is lower than 0.5 * maxEntropy, it is assumed
   * that the function is constant on [xMin, xMax].
   * The value of 0.5 has been chosen because of good experimental results but
   * could be tweaked. */
  constexpr T k_entropyThreshold = static_cast<T>(0.5);
  return entropy < k_maxEntropy * k_entropyThreshold;
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
  /* If the next step is toward zero, divide the postion, overwise multiply. */
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
T Solver<T>::nextPossibleRootInChild(const Tree* e, int childIndex) const {
  Solver<T> solver = *this;
  const Tree* child = e->child(childIndex);
  T xRoot;
  while (std::isfinite(
      xRoot = solver.nextRoot(child).x())) {  // assignment in condition
    /* Check the result in case another term is undefined,
     * e.g. (x+1)*ln(x) for x =- 1.
     *
     * Sometimes, xRoot is not precise enough and e can approximate to undef
     * even if it's not. e.g. f(x)=sqrt(cos(x)), when searching roots of cos(x),
     * sometimes we find cos(xRoot) = -0.0..01, so sqrt(cos(xRoot)) = undef.
     * To avoid this problem, clone e and replace cos(xRoot) by 0. */
    Tree* ebis = e->cloneTree();
    ebis->child(childIndex)->cloneTreeOverTree(0_e);
    /* This comparison relies on the fact that it is false for a NAN
     * approximation. */
    T value =
        Approximation::To<T>(ebis, xRoot,
                             Approximation::Parameters{.isRootAndCanHaveRandom =
                                                           true});  // m_unknown
    ebis->removeTree();
    if (std::fabs(value) < NullTolerance(xRoot)) {
      return xRoot;
    }
  }
  return k_NAN;
}

template <typename T>
T Solver<T>::nextRootInChildren(const Tree* e, ExpressionTestAuxiliary test,
                                void* aux) const {
  T xRoot = k_NAN;
  int n = e->numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (test(e->child(i), m_context, aux)) {
      T xRootChild = nextPossibleRootInChild(e, i);
      if (std::isfinite(xRootChild) &&
          (!std::isfinite(xRoot) ||
           std::fabs(m_xStart - xRootChild) < std::fabs(m_xStart - xRoot))) {
        xRoot = xRootChild;
      }
    }
  }
  return xRoot;
}

template <typename T>
T Solver<T>::nextRootInMultiplication(const Tree* e) const {
  assert(e->isMult());
  return nextRootInChildren(
      e, [](const Tree*, Context*, void*) { return true; }, nullptr);
}

template <typename T>
T Solver<T>::nextRootInAddition(const Tree* e) const {
  /* Special case for expressions of the form "f(x)^a+g(x)", with:
   * - f(x) and g(x) sharing a root x0
   * - f(x) being defined only on one side of x0
   * - 0 < a < 1
   * Since the expression does not change sign around x0, the usual numerical
   * schemes won't work. We instead look for the zeroes of f, and check whether
   * they are zeroes of the whole expression. */
  ExpressionTestAuxiliary test = [](const Tree* e, Context* context,
                                    void* aux) {
    /* TODO_PCJ: Either Pass ApproximationContext, ComplexFormat, AngleUnit and
     * SymbolicComputation::ReplaceDefinedSymbols or ensure
     * expression is projected. */
    return e->hasDescendantSatisfying([](const Tree* e) {
      T exponent = k_NAN;
      if (e->type() == Type::Sqrt) {
        exponent = static_cast<T>(0.5);
      } else if (e->type() == Type::Pow) {
        exponent =
            Approximation::To<T>(e->child(1), Approximation::Parameters{});
      } else if (e->type() == Type::Root) {
        exponent =
            static_cast<T>(1.) /
            Approximation::To<T>(e->child(1), Approximation::Parameters{});
      }
      if (std::isnan(exponent)) {
        return false;
      }
      return k_zero < exponent && exponent < static_cast<T>(1.);
    });
  };
  T xChildrenRoot = nextRootInChildren(e, test, const_cast<Solver<T>*>(this));
  Solver<T> solver = *this;
  T xRoot = solver.next(e, EvenOrOddRootInBracket, CompositeBrentForRoot).x();
  if (!std::isfinite(xRoot) ||
      std::fabs(xChildrenRoot - m_xStart) < std::fabs(xRoot - m_xStart)) {
    xRoot = xChildrenRoot;
  }
  return xRoot;
}

template <typename T>
T Solver<T>::nextRootInDependency(const Tree* e) const {
  assert(e->isDep());
  Solver<T> solver = *this;
  const Tree* main = Dependency::Main(e);
  // Find root in main
  Solution root = solver.nextRoot(main);
  // Check that the dependencies of the solution are not undefined
  while (root.interest() == Interest::Root &&
         std::isnan(Approximation::To<T>(
             e, root.x(),
             Approximation::Parameters{.isRootAndCanHaveRandom = true}))) {
    root = solver.nextRoot(main);
  }
  return root.x();
}

template <typename T>
T Solver<T>::MagicRound(T x) {
  constexpr T k_roundingOrder = 2. * k_minimalPracticalStep;  // Magic number
  return k_roundingOrder * std::round(x / k_roundingOrder);
}

template <typename T>
void Solver<T>::honeAndRoundSolution(
    FunctionEvaluation f, const void* aux, Coordinate2D<T> start,
    Coordinate2D<T> end, Interest interest, HoneResult hone,
    DiscontinuityEvaluation discontinuityTest) {
  assert(m_solutionQueue.isEmpty());
  if (interest == Interest::ReachedDiscontinuity) {
    return honeAndRoundDiscontinuitySolution(f, aux, start, end);
  }
  OMG::Troolean discontinuous = OMG::Troolean::Unknown;
  if (discontinuityTest) {
    discontinuous = discontinuityTest(start, end, aux) ? OMG::Troolean::True
                                                       : OMG::Troolean::False;
  }
  /* WARNING: This is a hack for discontinuous functions. BrentForRoot
   * needs to be very precise to find a root that is on the discontinuity bound.
   *
   * For other functions, we prefer a precision of NullTolerance because if the
   * precision is too high, the honing is more subject to approximations errors.
   */
  constexpr T precisionForDiscontinuousFunctions =
      k_relativePrecision * k_minimalAbsoluteStep;
  T xPrecision = discontinuous == OMG::Troolean::True
                     ? precisionForDiscontinuousFunctions
                     : NullTolerance(start.x());
  Coordinate2D<T> xy =
      hone(f, aux, start.x(), end.x(), interest, xPrecision, discontinuous);
  if (!std::isfinite(xy.x()) || !validSolution(xy.x())) {
    return;
  }

  T x = xy.x();
  /* When searching for an extremum, the function can take the extremum value
   * on several abscissas, and Brent can pick up any of them. This deviation
   * is particularly visible if the theoretical solution is an integer. */
  T roundX = MagicRound(x);
  T fRoundX = f(roundX, aux);
  if (std::isnan(fRoundX) && interest != Interest::Discontinuity) {
    // We might have a discontinuity, the solution might not be valid
    return;
  }
  // f(x) is different from the honed solution when searching intersections
  T fx = f(x, aux);
  if (std::isfinite(roundX) && validSolution(roundX)) {
    /* Filter out solutions that are close to a discontinuity. This can
     * happen with functions such as  y = (-x when x < 0, x-1 otherwise)
     * with which a root is found in (0,0) when it should not.
     * It is assumed that the piecewise condition are more often than not
     * integers or of a magnitude closer to roundX than x. So if the
     * condition of roundX is different from x, this means that the solution
     * found is probably on an open interval. */
    if (discontinuityTest &&
        discontinuityTest(Coordinate2D<T>(x, fx),
                          Coordinate2D<T>(roundX, fRoundX), aux)) {
      return;
    }
    if (fRoundX == fx ||
        (interest == Interest::Root && std::fabs(fRoundX) < std::fabs(fx)) ||
        (interest == Interest::LocalMinimum && fRoundX < fx) ||
        (interest == Interest::LocalMaximum && fRoundX > fx)) {
      // Round is better
      xy.setX(roundX);
      if (xy.y() == fx) {
        xy.setY(fRoundX);
      }
    }
  }
  assert(std::isfinite(xy.x()));
  if (std::isfinite(xy.y()) || interest == Interest::Discontinuity) {
    m_solutionQueue.push(Solution(xy, interest));
  }
}

template <typename T>
bool Solver<T>::DiscontinuityTestAtPoints(Coordinate2D<T> a, Coordinate2D<T> b,
                                          const void* aux) {
  if (std::isnan(a.y()) && std::isnan(b.y())) {
    return false;
  }
  if (std::isnan(a.y()) || std::isnan(b.y())) {
    return true;
  }
  return std::abs(b.y() - a.y()) >= NullTolerance(a.y()) &&
         DiscontinuityTestBetweenPoints(a, b, aux);
}

template <typename T>
void Solver<T>::honeAndRoundDiscontinuitySolution(FunctionEvaluation f,
                                                  const void* aux,
                                                  Coordinate2D<T> start,
                                                  Coordinate2D<T> end) {
  assert(m_solutionQueue.isEmpty());
  T precision = NullTolerance(start.x());

  // Find the smallest interval containing the discontinuity
  Coordinate2D<T> left = start;
  Coordinate2D<T> middle;
  Coordinate2D<T> right = end;
  if (!FindMinimalIntervalContainingDiscontinuity(f, aux, &left, &middle,
                                                  &right, precision)) {
    return;
  }

  // Round middle to find the discontinuity abscissa
  middle.setX(MagicRound(middle.x()));
  assert(std::isfinite(middle.x()));
  if (!validSolution(middle.x())) {
    return;
  }

  // Update left and right to make sure there are different from middle
  left.setX(middle.x() - precision / 2.);
  right.setX(middle.x() + precision / 2.);
  assert(middle.x() != left.x() && middle.x() != right.x());

  // Update ordinates
  middle.setY(f(middle.x(), aux));
  // We round because we want the "limit"
  left.setY(MagicRound(f(left.x(), aux)));
  right.setY(MagicRound(f(right.x(), aux)));

  bool leftIsDiscontinuous = DiscontinuityTestAtPoints(left, middle, aux);
  bool rightIsDiscontinuous = DiscontinuityTestAtPoints(middle, right, aux);
  if (!leftIsDiscontinuous && !rightIsDiscontinuous) {
    // No discontinuity
    return;
  }
  if (!leftIsDiscontinuous) {
    // No left discontinuity
    left = Coordinate2D<T>();
  }
  if (!rightIsDiscontinuous) {
    // No right discontinuity
    right = Coordinate2D<T>();
  }
  if (std::abs(right.y() - left.y()) < NullTolerance(middle.x())) {
    // Left and right are the same
    assert(leftIsDiscontinuous && rightIsDiscontinuous);
    // Keep only 1 point of interest
    left.setY(MagicRound((left.y() + right.y()) / 2.));
    right = Coordinate2D<T>();
  }

  // There should be at least one finite value
  assert(std::isfinite(left.y()) || std::isfinite(middle.y()) ||
         std::isfinite(right.y()));

  // Push solutions
  if (std::isfinite(left.y())) {
    left.setX(middle.x());
    m_solutionQueue.push(Solution(left, Interest::UnreachedDiscontinuity));
  }
  if (std::isfinite(middle.y())) {
    m_solutionQueue.push(Solution(middle, Interest::ReachedDiscontinuity));
  }
  if (std::isfinite(right.y())) {
    right.setX(middle.x());
    m_solutionQueue.push(Solution(right, Interest::UnreachedDiscontinuity));
  }
}

template <typename T>
typename Solver<T>::Solution Solver<T>::registerSolution(Solution solution,
                                                         bool wasQueued) {
  if (std::isnan(solution.x())) {
    return Solution();
  }
  assert((wasQueued && solution.x() == m_xStart) ||
         (!wasQueued && validSolution(solution.x())));
  if (std::fabs(solution.y()) < NullTolerance(solution.x())) {
    solution.setY(k_zero);
  }
  m_xStart = solution.x();
  assert((solution.interest() == Interest::None) == std::isnan(solution.x()));
  return solution;
}

template <typename T>
bool Solver<T>::FindMinimalIntervalContainingDiscontinuity(
    FunctionEvaluation f, const void* aux, Coordinate2D<T>* start,
    Coordinate2D<T>* middle, Coordinate2D<T>* end, T minimalSizeOfInterval) {
  assert(DiscontinuityTestBetweenPoints(*start, *end, aux));

  // Initialize middle if needed
  if (std::isnan(middle->x())) {
    middle->setX((start->x() + end->x()) / 2.0);
    middle->setY(f(middle->x(), aux));
  }

  while (end->x() - start->x() >= minimalSizeOfInterval) {
    bool leftIsDiscontinuous =
        DiscontinuityTestBetweenPoints(*start, *middle, aux);
    bool rightIsDiscontinuous =
        DiscontinuityTestBetweenPoints(*middle, *end, aux);
    if (leftIsDiscontinuous == rightIsDiscontinuous) {
      /* Either too many discontinuities and/or step is too big
       * Or couldn't find any discontinuities */
      return false;
    }
    if (leftIsDiscontinuous) {
      *end = *middle;
    } else {
      assert(rightIsDiscontinuous);
      *start = *middle;
    }
    middle->setX((start->x() + end->x()) / 2.0);
    middle->setY(f(middle->x(), aux));
    assert(DiscontinuityTestBetweenPoints(*start, *end, aux));
  }
  assert(start->x() <= end->x() &&
         end->x() - start->x() <= minimalSizeOfInterval &&
         DiscontinuityTestBetweenPoints(*start, *end, aux));
  return true;
}

// Explicit template instantiations

template Solver<double>::Solver(double, double, Context*);
template Solver<double>::Solution Solver<double>::next(
    FunctionEvaluation, const void*, BracketTest, HoneResult,
    DiscontinuityEvaluation discontinuityTest);
template Solver<double>::Solution Solver<double>::nextRoot(const Tree*);
template Solver<double>::Solution Solver<double>::nextMinimum(const Tree*);
template Solver<double>::Solution Solver<double>::nextIntersection(
    const Tree*, const Tree*, const Tree**);
template void Solver<double>::stretch();
template Coordinate2D<double> Solver<double>::SafeBrentMaximum(
    FunctionEvaluation, const void*, double, double, Interest, double,
    OMG::Troolean);
template double Solver<double>::DefaultSearchStepForAmplitude(double);
template void Solver<double>::ExcludeUndefinedFromBracket(
    Coordinate2D<double>* p1, Coordinate2D<double>* p2,
    Coordinate2D<double>* p3, FunctionEvaluation f, const void* aux,
    double minimalSizeOfInterval);
template bool Solver<double>::FunctionSeemsConstantOnTheInterval(
    Solver<double>::FunctionEvaluation f, const void* aux, double xMin,
    double xMax);

template Solver<float>::Interest Solver<float>::EvenOrOddRootInBracket(
    Coordinate2D<float>, Coordinate2D<float>, Coordinate2D<float>, const void*);
template Solver<float>::Solver(float, float, Context*);
template Solver<float>::Solution Solver<float>::next(
    FunctionEvaluation, const void*, BracketTest, HoneResult,
    DiscontinuityEvaluation discontinuityTest);
template float Solver<float>::DefaultSearchStepForAmplitude(float);

}  // namespace Poincare
