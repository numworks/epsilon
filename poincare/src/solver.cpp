#include <poincare/piecewise_operator.h>
#include <poincare/solver.h>
#include <poincare/solver_algorithms.h>
#include <poincare/subtraction.h>

namespace Poincare {

template <typename T>
Solver<T>::Solver(T xStart, T xEnd, const char *unknown, Context *context,
                  Preferences::ComplexFormat complexFormat,
                  Preferences::AngleUnit angleUnit)
    : m_xStart(xStart),
      m_xEnd(xEnd),
      m_maximalXStep(MaximalStep(xEnd - xStart)),
      m_yResult(k_NAN),
      m_context(context),
      m_unknown(unknown),
      m_complexFormat(complexFormat),
      m_angleUnit(angleUnit),
      m_lastInterest(Interest::None) {}

template <typename T>
Coordinate2D<T> Solver<T>::next(FunctionEvaluation f, const void *aux,
                                BracketTest test, HoneResult hone,
                                DiscontinuityEvaluation discontinuityTest) {
  Coordinate2D<T> p1, p2(start(), f(start(), aux)),
      p3(nextX(p2.x(), end(), static_cast<T>(1.)), k_NAN);
  p3.setY(f(p3.x(), aux));
  Coordinate2D<T> definitiveSolution;
  Interest definitiveInterest = Interest::None;

  constexpr bool isDouble = sizeof(T) == sizeof(double);

  while ((start() < p3.x()) == (p3.x() < end())) {
    p1 = p2;
    p2 = p3;
    /* If the solver is in float, the slope is not used by minimalStep
     * so its computation is skipped here. */
    T slope =
        isDouble ? (p2.y() - p1.y()) / (p2.x() - p1.x()) : static_cast<T>(1.);
    p3.setX(nextX(p2.x(), end(), slope));
    p3.setY(f(p3.x(), aux));

    Coordinate2D<T> start = p1;
    Coordinate2D<T> middle = p2;
    Coordinate2D<T> end = p3;
    Interest interest = Interest::None;
    /* If the solver is in float, we want it to be fast so the fine search
     * of interest around undefined intervals is skipped. */
    if ((interest = test(start, middle, end, aux)) ==
            Interest::None &&  // assignment in condition
        isDouble &&
        UndefinedInBracket(start, middle, end, aux) ==
            Interest::Discontinuity) {
      /* If no interest was found and there is an undefined subinterval in the
       * interval, search for the largest interval without undef and then
       * recompute the interest in this interval.
       * */
      ExcludeUndefinedFromBracket(&start, &middle, &end, f, aux,
                                  MinimalStep(middle.x(), slope));
      interest = test(start, middle, end, aux);
    }

    if (interest != Interest::None) {
      Coordinate2D<T> solution = honeAndRoundSolution(
          f, aux, start.x(), end.x(), interest, hone, discontinuityTest);
      if (std::isfinite(solution.x()) && validSolution(solution.x())) {
        definitiveSolution = solution;
        definitiveInterest = interest;
        break;
      }
    }
  }

  registerSolution(definitiveSolution, definitiveInterest);
  return result();
}

template <typename T>
Coordinate2D<T> Solver<T>::next(const Expression &e, BracketTest test,
                                HoneResult hone) {
  assert(m_unknown && m_unknown[0] != '\0');
  if (e.recursivelyMatches(Expression::IsRandom, m_context)) {
    return Coordinate2D<T>(NAN, NAN);
  }
  FunctionEvaluationParameters parameters = {.context = m_context,
                                             .unknown = m_unknown,
                                             .expression = e,
                                             .complexFormat = m_complexFormat,
                                             .angleUnit = m_angleUnit};
  FunctionEvaluation f = [](T x, const void *aux) {
    const FunctionEvaluationParameters *p =
        reinterpret_cast<const FunctionEvaluationParameters *>(aux);
    return p->expression.approximateWithValueForSymbol(
        p->unknown, x, p->context, p->complexFormat, p->angleUnit);
  };
  DiscontinuityEvaluation discontinuityTestForPiecewise = [](T x1, T x2,
                                                             const void *aux) {
    const FunctionEvaluationParameters *p =
        reinterpret_cast<const FunctionEvaluationParameters *>(aux);
    assert(p->expression.type() == ExpressionNode::Type::PiecewiseOperator);
    const PiecewiseOperator piecewise =
        static_cast<const PiecewiseOperator &>(p->expression);
    return piecewise.indexOfFirstTrueConditionWithValueForSymbol<T>(
               p->unknown, x1, p->context, p->complexFormat, p->angleUnit) !=
           piecewise.indexOfFirstTrueConditionWithValueForSymbol(
               p->unknown, x2, p->context, p->complexFormat, p->angleUnit);
  };

  return next(f, &parameters, test, hone,
              e.type() == ExpressionNode::Type::PiecewiseOperator
                  ? discontinuityTestForPiecewise
                  : nullptr);
}

template <typename T>
Coordinate2D<T> Solver<T>::nextRoot(const Expression &e) {
  if (e.recursivelyMatches(Expression::IsRandom, m_context)) {
    return Coordinate2D<T>(NAN, NAN);
  }
  ExpressionNode::Type type = e.type();

  switch (type) {
    case ExpressionNode::Type::Multiplication:
      /* x*y = 0 => x = 0 or y = 0 */
      registerSolution(nextRootInMultiplication(e), Interest::Root);
      return result();

    case ExpressionNode::Type::Addition:
    case ExpressionNode::Type::Subtraction:
      registerSolution(nextRootInAddition(e), Interest::Root);
      return result();

    case ExpressionNode::Type::Power:
    case ExpressionNode::Type::NthRoot:
    case ExpressionNode::Type::Division:
      /* f(x,y) = 0 => x = 0 */
      registerSolution(nextPossibleRootInChild(e, 0), Interest::Root);
      return result();

    case ExpressionNode::Type::AbsoluteValue:
    case ExpressionNode::Type::HyperbolicSine:
    case ExpressionNode::Type::Opposite:
    case ExpressionNode::Type::SquareRoot:
      /* f(x) = 0 <=> x = 0 */
      return nextRoot(e.childAtIndex(0));

    default:
      if (e.isNull(m_context) == TrinaryBoolean::False) {
        registerSolution(Coordinate2D<T>(), Interest::None);
        return Coordinate2D<T>();
      }

      Coordinate2D<T> res =
          next(e, EvenOrOddRootInBracket, CompositeBrentForRoot);
      if (lastInterest() != Interest::None) {
        m_lastInterest = Interest::Root;
      }
      return res;
  }
}

template <typename T>
Coordinate2D<T> Solver<T>::nextMinimum(const Expression &e) {
  /* TODO We could add a layer of formal resolution:
   * - use the derivative (could be an optional argument to avoid recomputing
   *   it every time)
   * - since d(f°g) = dg×df°g, if f is known to be monotonous (i.e. df≠0), look
   *   for the extrema of g. */
  return next(e, MinimumInBracket, SafeBrentMinimum);
}

template <typename T>
Coordinate2D<T> Solver<T>::nextIntersection(const Expression &e1,
                                            const Expression &e2,
                                            Expression *memoizedDifference) {
  if (!memoizedDifference) {
    Expression diff;
    return nextIntersection(e1, e2, &diff);
  }
  assert(memoizedDifference);
  if (memoizedDifference->isUninitialized()) {
    ReductionContext reductionContext(m_context, m_complexFormat, m_angleUnit,
                                      Preferences::UnitFormat::Metric,
                                      ReductionTarget::SystemForAnalysis);
    *memoizedDifference = Subtraction::Builder(e1.clone(), e2.clone())
                              .cloneAndSimplify(reductionContext);
  }
  nextRoot(*memoizedDifference);
  if (m_lastInterest == Interest::Root) {
    m_lastInterest = Interest::Intersection;
    m_yResult = e1.approximateWithValueForSymbol<T>(
        m_unknown, m_xStart, m_context, m_complexFormat, m_angleUnit);
  }
  return result();
}

template <typename T>
void Solver<T>::stretch() {
  T step = maximalStep();
  T stepSign = m_xStart < m_xEnd ? static_cast<T>(1.) : static_cast<T>(-1.);
  m_xStart -= step * stepSign;
  m_xEnd += step * stepSign;
}

template <typename T>
typename Solver<T>::Interest Solver<T>::EvenOrOddRootInBracket(
    Coordinate2D<T> a, Coordinate2D<T> b, Coordinate2D<T> c, const void *aux) {
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
                                            const void *aux, T xMin, T xMax,
                                            Interest interest, T precision) {
  if (xMax < xMin) {
    return SafeBrentMinimum(f, aux, xMax, xMin, interest, precision);
  }
  assert(xMin < xMax);

  if (FunctionSeemsConstantOnTheInterval(f, aux, xMin, xMax)) {
    /* Some fake minimums can be detected due to approximations errors like in
     * f(x) = x/abs(x) in complex mode. */
    return Coordinate2D<T>(NAN, NAN);
  }

  return SolverAlgorithms::BrentMinimum(f, aux, xMin, xMax, interest,
                                        precision);
}

template <typename T>
Coordinate2D<T> Solver<T>::SafeBrentMaximum(FunctionEvaluation f,
                                            const void *aux, T xMin, T xMax,
                                            Interest interest, T precision) {
  const void *pack[] = {&f, aux};
  FunctionEvaluation minusF = [](T x, const void *aux) {
    const void *const *param = reinterpret_cast<const void *const *>(aux);
    const FunctionEvaluation *f =
        reinterpret_cast<const FunctionEvaluation *>(param[0]);
    return -(*f)(x, param[1]);
  };
  Coordinate2D<T> res =
      SafeBrentMinimum(minusF, pack, xMin, xMax, interest, precision);
  return Coordinate2D<T>(res.x(), -res.y());
}

template <typename T>
Coordinate2D<T> Solver<T>::CompositeBrentForRoot(FunctionEvaluation f,
                                                 const void *aux, T xMin,
                                                 T xMax, Interest interest,
                                                 T precision) {
  if (interest == Interest::Root) {
    Coordinate2D<T> solution =
        SolverAlgorithms::BrentRoot(f, aux, xMin, xMax, interest, precision);
    /* Filter out roots that do not evaluate to zero. This can happen with
     * functions such as y=floor(x)-0.5. */
    return IsOddRoot(solution, f, aux) ? solution : Coordinate2D<T>();
  }
  Coordinate2D<T> res;
  if (interest == Interest::LocalMinimum) {
    res = SafeBrentMinimum(f, aux, xMin, xMax, interest, precision);
  } else {
    assert(interest == Interest::LocalMaximum);
    res = SafeBrentMaximum(f, aux, xMin, xMax, interest, precision);
  }
  if (std::isfinite(res.x()) && std::fabs(res.y()) < NullTolerance(res.x())) {
    return res;
  }
  return std::isfinite(res.x()) && std::fabs(res.y()) < NullTolerance(res.x())
             ? res
             : Coordinate2D<T>();
}

template <typename T>
bool Solver<T>::IsOddRoot(Coordinate2D<T> root, FunctionEvaluation f,
                          const void *aux) {
  T b = root.x();
  T fb = root.y();
  if (!std::isfinite(fb)) {
    return false;
  }
  if (std::fabs(fb) < NullTolerance(b)) {
    return true;
  }
  T step = MinimalStep(b);
  T a = b, c = b, fa = fb, fc = fb;
  int i = 0;
  do {
    a = a - step;
    fa = f(a, aux);
    c = c + step;
    fc = f(c, aux);
  } while ((fa > k_zero) == (fc > k_zero) && ++i < 1000);
  T fab = std::fabs(fa - fb);
  T fcb = std::fabs(fc - fb);
  constexpr T k_magnitude = static_cast<T>(10.);  // magic number
  return i < 1000 && std::max(fab, fcb) < k_magnitude * std::min(fab, fcb);
}

template <typename T>
void Solver<T>::ExcludeUndefinedFromBracket(
    Coordinate2D<T> *p1, Coordinate2D<T> *p2, Coordinate2D<T> *p3,
    FunctionEvaluation f, const void *aux, T minimalSizeOfInterval) {
  assert(UndefinedInBracket(*p1, *p2, *p3, aux) == Interest::Discontinuity);
  /* Search for the smallest interval that contains the undefined and
   * return the largest interval that does not intersect with it. */
  Coordinate2D<T> dummy(k_NAN, k_NAN);
  Coordinate2D<T> lowerBoundOfDiscontinuity = *p1;
  Coordinate2D<T> middleOfDiscontinuity = *p2;
  Coordinate2D<T> upperBoundOfDiscontinuity = *p3;
  while (upperBoundOfDiscontinuity.x() - lowerBoundOfDiscontinuity.x() >=
         minimalSizeOfInterval) {
    if (UndefinedInBracket(lowerBoundOfDiscontinuity, dummy,
                           middleOfDiscontinuity,
                           aux) == Interest::Discontinuity) {
      upperBoundOfDiscontinuity = middleOfDiscontinuity;
      middleOfDiscontinuity.setX(
          (lowerBoundOfDiscontinuity.x() + middleOfDiscontinuity.x()) / 2.0);
      middleOfDiscontinuity.setY(f(middleOfDiscontinuity.x(), aux));
    } else if (UndefinedInBracket(middleOfDiscontinuity, dummy,
                                  upperBoundOfDiscontinuity,
                                  aux) == Interest::Discontinuity) {
      lowerBoundOfDiscontinuity = middleOfDiscontinuity;
      middleOfDiscontinuity.setX(
          (middleOfDiscontinuity.x() + upperBoundOfDiscontinuity.x()) / 2.0);
      middleOfDiscontinuity.setY(f(middleOfDiscontinuity.x(), aux));
    } else {
      /* This can happen if std::isinf(middleOfDiscontinuity), in which case no
       * smaller interval with the undefined can be found. */
      assert(std::isinf(middleOfDiscontinuity.y()));
      break;
    }
    // assert that dummy has no impact
    assert(UndefinedInBracket(lowerBoundOfDiscontinuity, middleOfDiscontinuity,
                              upperBoundOfDiscontinuity,
                              aux) == Interest::Discontinuity);
  }
  /* The smallest interval containing the undefined is found. Now
   * set p1, p2 and p3 outside of it. */
  if (std::isnan(lowerBoundOfDiscontinuity.y())) {
    *p1 = upperBoundOfDiscontinuity;
  } else {
    assert(std::isnan(upperBoundOfDiscontinuity.y()));
    *p3 = lowerBoundOfDiscontinuity;
  }
  p2->setX((p1->x() + p3->x()) / 2.0);
  p2->setY(f(p2->x(), aux));
}

template <typename T>
bool Solver<T>::FunctionSeemsConstantOnTheInterval(
    Solver<T>::FunctionEvaluation f, const void *aux, T xMin, T xMax) {
  assert(xMin < xMax);
  constexpr int k_numberOfSteps = 20;
  T values[k_numberOfSteps];
  int valuesCount[k_numberOfSteps];
  int currentNumberOfValues = 0;
  /* This loop computes 20 values of f on the interval and then checks the
   * repartition of these values. If the function takes a few number of
   * different values, it might mean that f is a constant function but
   * approximation errors led to thinking there was a minimum in the interval.
   * To mesure this "repartition" of values, the entropy of the data is
   * then calculated.
   * */
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
   * could be tweaked.
   * */
  constexpr T k_entropyThreshold = static_cast<T>(0.5);
  return entropy < k_maxEntropy * k_entropyThreshold;
}

template <typename T>
T Solver<T>::MaximalStep(T intervalAmplitude) {
  constexpr T minimalNumberOfSteps = static_cast<T>(100.);
  return std::max(k_minimalPracticalStep,
                  std::fabs(intervalAmplitude) / minimalNumberOfSteps);
}

template <typename T>
T Solver<T>::MinimalStep(T x, T slope) {
  T minimalStep = k_minimalPracticalStep;
  constexpr bool preventTooSmallStep = sizeof(T) == sizeof(double);
  if (preventTooSmallStep) {
    /* We make the minimal step dependent on the slope because if a function is
     * too flat, taking a step too small could lead to not detecting minimums
     * and maximums. Indeed, the function SolverAlgorithms::DetectApproxima-
     * -tionErrorsForMinimum filters out mins and maxs when the function
     * seems constant on the interval.
     * We use e^7 because e^7 ~ 1000, so that if slope = 0,
     * minStep = 10e-6 * 10e3 = 0.001.
     * This is not applied to floats since the minStep is already big enough.
     * */
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
   * - always sample a minimal number of points in the whole interval. */
  constexpr T baseGrowthSpeed = static_cast<T>(1.05);
  static_assert(baseGrowthSpeed > static_cast<T>(1.),
                "Growth speed must be greater than 1");
  constexpr T maximalGrowthSpeed = static_cast<T>(10.);
  constexpr T growthSpeedAcceleration = static_cast<T>(1e-2);
  /* Increase density between 0.1 and 100 */
  constexpr T lowerTypicalMagnitude = static_cast<T>(-1.);
  constexpr T upperTypicalMagnitude = static_cast<T>(3.);

  T maxStep = maximalStep();
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
Coordinate2D<T> Solver<T>::nextPossibleRootInChild(const Expression &e,
                                                   int childIndex) const {
  Solver<T> solver = *this;
  Expression child = e.childAtIndex(childIndex);
  T xRoot;
  while (std::isfinite(
      xRoot = solver.nextRoot(child).x())) {  // assignment in condition
    /* Check the result in case another term is undefined,
     * e.g. (x+1)*ln(x) for x =- 1.
     * This comparison relies on the fact that it is false for a NAN
     * approximation. */
    if (std::fabs(e.approximateWithValueForSymbol<T>(
            m_unknown, xRoot, m_context, m_complexFormat, m_angleUnit)) <
        NullTolerance(xRoot)) {
      return Coordinate2D<T>(xRoot, k_zero);
    }
  }
  return Coordinate2D<T>(k_NAN, k_NAN);
}

template <typename T>
Coordinate2D<T> Solver<T>::nextRootInChildren(
    const Expression &e, Expression::ExpressionTestAuxiliary test,
    void *aux) const {
  T xRoot = k_NAN;
  int n = e.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (test(e.childAtIndex(i), m_context, aux)) {
      T xRootChild = nextPossibleRootInChild(e, i).x();
      if (std::isfinite(xRootChild) &&
          (!std::isfinite(xRoot) ||
           std::fabs(m_xStart - xRootChild) < std::fabs(m_xStart - xRoot))) {
        xRoot = xRootChild;
      }
    }
  }
  return Coordinate2D<T>(xRoot, k_zero);
}

template <typename T>
Coordinate2D<T> Solver<T>::nextRootInMultiplication(const Expression &e) const {
  assert(e.type() == ExpressionNode::Type::Multiplication);
  return nextRootInChildren(
      e, [](const Expression, Context *, void *) { return true; }, nullptr);
}

template <typename T>
Coordinate2D<T> Solver<T>::nextRootInAddition(const Expression &e) const {
  /* Special case for expressions of the form "f(x)^a+g(x)", with:
   * - f(x) and g(x) sharing a root x0
   * - f(x) being defined only on one side of x0
   * - 0 < a < 1
   * Since the expression does not change sign around x0, the usual numerical
   * schemes won't work. We instead look for the zeroes of f, and check whether
   * they are zeroes of the whole expression. */
  Expression::ExpressionTestAuxiliary test = [](const Expression e,
                                                Context *context, void *aux) {
    return e.recursivelyMatches(
        [](const Expression e, Context *context, void *aux) {
          const Solver<T> *solver = static_cast<const Solver<T> *>(aux);
          T exponent = k_NAN;
          if (e.type() == ExpressionNode::Type::SquareRoot) {
            exponent = static_cast<T>(0.5);
          } else if (e.type() == ExpressionNode::Type::Power) {
            exponent = e.childAtIndex(1).approximateToScalar<T>(
                context, solver->m_complexFormat, solver->m_angleUnit);
          } else if (e.type() == ExpressionNode::Type::NthRoot) {
            exponent =
                static_cast<T>(1.) /
                e.childAtIndex(1).approximateToScalar<T>(
                    context, solver->m_complexFormat, solver->m_angleUnit);
          }
          if (std::isnan(exponent)) {
            return false;
          }
          return k_zero < exponent && exponent < static_cast<T>(1.);
        },
        context, SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
        aux);
  };
  T xChildrenRoot =
      nextRootInChildren(e, test, const_cast<Solver<T> *>(this)).x();
  Solver<T> solver = *this;
  T xRoot = solver.next(e, EvenOrOddRootInBracket, CompositeBrentForRoot).x();
  if (!std::isfinite(xRoot) ||
      std::fabs(xChildrenRoot - m_xStart) < std::fabs(xRoot - m_xStart)) {
    xRoot = xChildrenRoot;
  }
  return Coordinate2D<T>(xRoot, k_zero);
}

template <typename T>
Coordinate2D<T> Solver<T>::honeAndRoundSolution(
    FunctionEvaluation f, const void *aux, T start, T end, Interest interest,
    HoneResult hone, DiscontinuityEvaluation discontinuityTest) {
  Coordinate2D<T> solution =
      hone(f, aux, start, end, interest, k_absolutePrecision);
  if (!std::isfinite(solution.x()) || !validSolution(solution.x())) {
    return solution;
  }

  T x = solution.x();
  /* When searching for an extremum, the function can take the extremal value
   * on several abscissas, and Brent can pick up any of them. This deviation
   * is particularly visible if the theoretical solution is an integer. */
  constexpr T k_roundingOrder = 2. * k_minimalPracticalStep;  // Magic number
  T roundX = k_roundingOrder * std::round(x / k_roundingOrder);
  if (std::isfinite(roundX) && validSolution(roundX)) {
    T fIntX = f(roundX, aux);
    T fx = f(x, aux);  // f(x) is different from the honed solution when
                       // searching intersections
    /* Filter out solutions that are close to a discontinuity. This can
     * happen with functions such as  y = (-x when x < 0, x-1 otherwise)
     * with which a root is found in (0,0) when it should not.
     * It is assumed that the piecewise condition are more often than not
     * integers or of a magnitude closer to roundX than x. So if the
     * condition of roundX is different from x, this means that the solution
     * found is probably on an open interval.
     * */
    if (discontinuityTest && discontinuityTest(x, roundX, aux)) {
      solution = Coordinate2D<T>(k_NAN, k_NAN);
    } else if (fIntX == fx ||
               (interest == Interest::Root &&
                std::fabs(fIntX) < std::fabs(fx)) ||
               (interest == Interest::LocalMinimum && fIntX < fx) ||
               (interest == Interest::LocalMaximum && fIntX > fx)) {
      // Round is better
      solution.setX(roundX);
    }
  }
  return solution;
}

template <typename T>
void Solver<T>::registerSolution(Coordinate2D<T> solution, Interest interest) {
  if (std::isnan(solution.x())) {
    m_lastInterest = Interest::None;
    m_xStart = k_NAN;
    m_yResult = k_NAN;
    return;
  }
  assert(validSolution(solution.x()));
  m_xStart = solution.x();
  m_yResult = solution.y();
  if (std::fabs(m_yResult) < NullTolerance(solution.x())) {
    m_yResult = k_zero;
  }
  m_lastInterest = interest;
}

// Explicit template instanciations

template Solver<double>::Solver(double, double, const char *, Context *,
                                Preferences::ComplexFormat,
                                Preferences::AngleUnit);
template Coordinate2D<double> Solver<double>::next(
    FunctionEvaluation, const void *, BracketTest, HoneResult,
    DiscontinuityEvaluation discontinuityTest);
template Coordinate2D<double> Solver<double>::nextRoot(const Expression &);
template Coordinate2D<double> Solver<double>::nextMinimum(const Expression &);
template Coordinate2D<double> Solver<double>::nextIntersection(
    const Expression &, const Expression &, Expression *);
template void Solver<double>::stretch();
template Coordinate2D<double> Solver<double>::SafeBrentMaximum(
    FunctionEvaluation, const void *, double, double, Interest, double);
template double Solver<double>::MaximalStep(double);
template void Solver<double>::ExcludeUndefinedFromBracket(
    Coordinate2D<double> *p1, Coordinate2D<double> *p2,
    Coordinate2D<double> *p3, FunctionEvaluation f, const void *aux,
    double minimalSizeOfInterval);
template bool Solver<double>::FunctionSeemsConstantOnTheInterval(
    Solver<double>::FunctionEvaluation f, const void *aux, double xMin,
    double xMax);

template Solver<float>::Interest Solver<float>::EvenOrOddRootInBracket(
    Coordinate2D<float>, Coordinate2D<float>, Coordinate2D<float>,
    const void *);
template Solver<float>::Solver(float, float, const char *, Context *,
                               Preferences::ComplexFormat,
                               Preferences::AngleUnit);
template Coordinate2D<float> Solver<float>::next(
    FunctionEvaluation, const void *, BracketTest, HoneResult,
    DiscontinuityEvaluation discontinuityTest);
template float Solver<float>::MaximalStep(float);

}  // namespace Poincare
