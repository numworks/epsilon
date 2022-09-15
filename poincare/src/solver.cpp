#include <poincare/solver.h>
#include <poincare/subtraction.h>
#include <poincare/solver_algorithms.h>

namespace Poincare {

template<typename T>
Solver<T>::Solver(T xStart, T xEnd, const char * unknown, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, T precision) :
  m_xStart(xStart),
  m_xEnd(xEnd),
  m_yResult(k_NAN),
  m_precision(precision),
  m_context(context),
  m_unknown(unknown),
  m_complexFormat(complexFormat),
  m_angleUnit(angleUnit),
  m_lastInterest(Interest::None)
{}

template<typename T>
Solver<T>::Solver(const Solver<T> * other) :
  Solver(other->m_xStart, other->m_xEnd, other->m_unknown, other->m_context, other->m_complexFormat, other->m_angleUnit, other->m_precision)
{}

template<typename T>
Coordinate2D<T> Solver<T>::next(FunctionEvaluation f, const void * aux, BracketTest test, HoneResult hone) {
  T x1, x2 = start(), x3 = nextX(x2, end());
  T y1, y2 = f(x2, aux), y3 = f(x3, aux);
  Coordinate2D<T> solution(k_NAN, k_NAN);
  Interest interest = Interest::None;

  do {
    x1 = x2;
    x2 = x3;
    x3 = nextX(x2, end());
    y1 = y2;
    y2 = y3;
    y3 = f(x3, aux);

    interest = test(y1, y2, y3);
    if (interest != Interest::None) {
      solution = hone(f, aux, x1, x3, interest, m_precision);
      if (std::isfinite(solution.x1()) && solution.x1() != start()) {
        break;
      }
    }
  } while ((start() < x3) == (x3 < end()));

  registerSolution(solution, interest);
  return result();
}

template<typename T>
Coordinate2D<T> Solver<T>::next(Expression e, BracketTest test, HoneResult hone) {
  assert(m_unknown && m_unknown[0] != '\0');
  FunctionEvaluationParameters parameters = { .context = m_context, .unknown = m_unknown, .expression = e, .complexFormat = m_complexFormat, .angleUnit = m_angleUnit };
  FunctionEvaluation f = [](T x, const void * aux) {
    const FunctionEvaluationParameters * p = reinterpret_cast<const FunctionEvaluationParameters *>(aux);
    return p->expression.approximateWithValueForSymbol(p->unknown, x, p->context, p->complexFormat, p->angleUnit);
  };

  return next(f, &parameters, test, hone);
}

template<typename T>
Coordinate2D<T> Solver<T>::nextRoot(Expression e) {
  ExpressionNode::Type type = e.type();

  switch (type) {
  case ExpressionNode::Type::Multiplication:
    /* x*y = 0 => x = 0 or y = 0
     * Check the result in case another term is undefined,
     * e.g. (x+1)*ln(x) for x =- 1*/
    registerSolution(nextRootInMultiplication(e), Interest::Root);
    return result();

  case ExpressionNode::Type::Power:
  case ExpressionNode::Type::NthRoot:
  case ExpressionNode::Type::Division:
    /* f(x,y) = 0 => x = 0
     * Check the result as some values of y may invalidate it (e.g. x^(1/x)
     * or (x-1)/ln(x)). */
    registerSolution(nextPossibleRootInChild(e, 0), Interest::Root);
    return result();

  case ExpressionNode::Type::AbsoluteValue:
  case ExpressionNode::Type::HyperbolicSine:
  case ExpressionNode::Type::Opposite:
  case ExpressionNode::Type::SquareRoot:
    /* f(x) = 0 <=> x = 0 */
    return nextRoot(e.childAtIndex(0));

  default:
    Coordinate2D<T> res = next(e, EvenOrOddRootInBracket, CompositeBrentForRoot);
    if (lastInterest() != Interest::None) {
      m_lastInterest = Interest::Root;
    }
    return res;
  }
}

template<typename T>
Coordinate2D<T> Solver<T>::nextMinimum(Expression e) {
  return next(e, MinimumInBracket, SolverAlgorithms::BrentMinimum);
}

template<typename T>
Coordinate2D<T> Solver<T>::nextIntersection(Expression e1, Expression e2) {
  Expression subtraction = Subtraction::Builder(e1, e2);
  ExpressionNode::ReductionContext reductionContext(m_context, m_complexFormat, m_angleUnit, Preferences::UnitFormat::Metric, ExpressionNode::ReductionTarget::SystemForAnalysis);
  subtraction = subtraction.cloneAndSimplify(reductionContext);
  nextRoot(subtraction);
  if (m_lastInterest == Interest::Root) {
    m_lastInterest = Interest::Intersection;
    m_yResult = e1.approximateWithValueForSymbol<T>(m_unknown, m_xStart, m_context, m_complexFormat, m_angleUnit);
  }
  return result();
}

template<typename T>
void Solver<T>::stretch() {
  T step = maximalStep();
  T stepSign = m_xStart < m_xEnd ? static_cast<T>(1.) : static_cast<T>(-1.);
  m_xStart -= step * stepSign;
  m_xEnd += step * stepSign;
}

template<typename T>
typename Solver<T>::Interest Solver<T>::EvenOrOddRootInBracket(T a, T b, T c) {
  Interest root = OddRootInBracket(a, b, c);
  if (root != Interest::None) {
    return root;
  }
  /* FIXME Check the sign of a,b and c. A minimum can only be a root if b is
   * positive. */
  Interest extremum = MinimumInBracket(a, b, c);
  return extremum == Interest::None ? MaximumInBracket(a, b, c) : extremum;
}

template<typename T>
Coordinate2D<T> Solver<T>::BrentMaximum(FunctionEvaluation f, const void * aux, T xMin, T xMax, Interest interest, T precision) {
  const void * pack[] = {&f, aux};
  FunctionEvaluation minusF = [](T x, const void * aux) {
    const void * const * param = reinterpret_cast<const void * const *>(aux);
    const FunctionEvaluation * f = reinterpret_cast<const FunctionEvaluation *>(param[0]);
    return -(*f)(x, param[1]);
  };
  Coordinate2D<T> res = SolverAlgorithms::BrentMinimum(minusF, pack, xMin, xMax, interest, precision);
  return Coordinate2D<T>(res.x1(), -res.x2());
}

template<typename T>
Coordinate2D<T> Solver<T>::CompositeBrentForRoot(FunctionEvaluation f, const void * aux, T xMin, T xMax, Interest interest, T precision) {
  if (interest == Interest::Root) {
    return SolverAlgorithms::BrentRoot(f, aux, xMin, xMax, interest, precision);
  }
  Coordinate2D<T> res;
  if (interest == Interest::LocalMinimum) {
    res = SolverAlgorithms::BrentMinimum(f, aux, xMin, xMax, interest, precision);
  } else {
    assert(interest == Interest::LocalMaximum);
    res = BrentMaximum(f, aux, xMin, xMax, interest, precision);
  }
  if (std::isfinite(res.x1()) && std::fabs(res.x2()) < NullTolerance(precision)) {
    return res;
  }
  return Coordinate2D<T>(k_NAN, k_NAN);
}

template<typename T>
T Solver<T>::maximalStep() const {
  constexpr T minimalNumberOfSteps = static_cast<T>(100.);
  return std::max(k_minimalAbsoluteStep, std::fabs(m_xEnd - m_xStart) / minimalNumberOfSteps);
}

template<typename T>
T Solver<T>::nextX(T x, T direction) const {
  /* Compute the next step for the bracketing algorithm. The formula is derived
   * from the following criteria:
   * - using a fixed step would either lead to poor precision close to zero or
   *   prohibitive computation times on large intervals.
   * - we assume that for a typical function, distance between to points of
   *   interest is of the same magnitude as their abscissa ; as such, we want
   *   to sample with the same density on ]-10,10[ and ]-1000,1000[.
   * - we further assume that for a typical *high-school* function, points of
   *   interest are more likely to be close to the unit, rather than be
   *   around 1e6 or 1e-4.
   *
   * As such we use a formula of the form t+dt = t * (φ(log|t|) + α)
   * The geometric growth ensures we do not over-sample on large intervals,
   * and the term φ allows increasing the ratio in "less interesting" areas.
   *
   * As for the limits, we ensure that:
   * - even when |t| is large, dt never skips an order of magnitude
   *   i.e. 0.1 < |(t+dt)/t| < 10
   * - there is a minimal value for dt, to allow crossing zero.
   * - always sample a minimal number of points in the whole interval. */
  constexpr T baseGrowthSpeed = static_cast<T>(1.1);
  static_assert(baseGrowthSpeed > static_cast<T>(1.), "Growth speed must be greater than 1");
  constexpr T maximalGrowthSpeed = static_cast<T>(10.);
  constexpr T growthSpeedAcceleration = static_cast<T>(1e-2);
  /* Increase density between 0.1 and 100 */
  constexpr T lowerTypicalMagnitude = static_cast<T>(-1.);
  constexpr T upperTypicalMagnitude = static_cast<T>(2.);

  T maxStep = maximalStep();
  T stepSign = x < direction ? static_cast<T>(1.) : static_cast<T>(-1.);

  T magnitude = std::log(std::fabs(x));
  if (!std::isfinite(magnitude)) {
    return x + stepSign * k_minimalAbsoluteStep;
  }
  T ratio;
  if (lowerTypicalMagnitude < magnitude && magnitude < upperTypicalMagnitude) {
    ratio = baseGrowthSpeed;
  } else {
    T exponent = magnitude < lowerTypicalMagnitude ? magnitude - lowerTypicalMagnitude : upperTypicalMagnitude - magnitude;
    ratio = maximalGrowthSpeed - (maximalGrowthSpeed - baseGrowthSpeed) * std::exp(growthSpeedAcceleration * std::pow(exponent, 3.));
  }
  T x2 = (x < direction) == (x < k_zero) ? x / ratio : x * ratio;
  if (std::fabs(x - x2) < k_minimalAbsoluteStep) {
    x2 = x + stepSign * k_minimalAbsoluteStep;
  } else if (std::fabs(x - x2) > maxStep) {
    x2 = x + stepSign * maxStep;
  }
  assert((x < x2) == (x < direction));
  assert(x2 != x);
  return x2;
}

template<typename T>
Coordinate2D<T> Solver<T>::nextPossibleRootInChild(Expression e, int childIndex) const {
  Solver<T> solver(this);
  Expression child = e.childAtIndex(childIndex);
  T xRoot = solver.nextRoot(child).x1();
  while (std::isfinite(xRoot)) {
    if (std::fabs(e.approximateWithValueForSymbol<T>(m_unknown, xRoot, m_context, m_complexFormat, m_angleUnit)) < NullTolerance(m_precision)) {
      return Coordinate2D<T>(xRoot, k_zero);
    }
    xRoot = solver.nextRoot(child).x1();
  }
  return Coordinate2D<T>(k_NAN, k_NAN);
}

template<typename T>
Coordinate2D<T> Solver<T>::nextRootInMultiplication(Expression e) const {
  assert(e.type() == ExpressionNode::Type::Multiplication);
  T xRoot = k_NAN;
  int n = e.numberOfChildren();
  for (int i = 0; i < n; i++) {
    T xRootChild = nextPossibleRootInChild(e, i).x1();
    if (std::isfinite(xRootChild) && (!std::isfinite(xRoot) || std::fabs(m_xStart - xRootChild) < std::fabs(m_xStart - xRoot))) {
      xRoot = xRootChild;
    }
  }
  return Coordinate2D<T>(xRoot, k_zero);
}

template<typename T>
void Solver<T>::registerSolution(Coordinate2D<T> solution, Interest interest) {
  T x = solution.x1();
  if ((m_xStart < x && x < m_xEnd) || (m_xEnd < x && x < m_xStart)) {
    m_xStart = x;
    m_yResult = solution.x2();
    m_lastInterest = interest;
  } else {
    m_lastInterest = Interest::None;
  }
}

// Explicit template instanciations

template Solver<double>::Solver(double, double, const char *, Context *, Preferences::ComplexFormat, Preferences::AngleUnit, double);
template Coordinate2D<double> Solver<double>::nextRoot(Expression);
template Coordinate2D<double> Solver<double>::nextMinimum(Expression);
template Coordinate2D<double> Solver<double>::nextIntersection(Expression, Expression);
template void Solver<double>::stretch();

#if 1
template<typename T>
Coordinate2D<T> SolverHelper<T>::NextPointOfInterest(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, BracketSearch search, T start, T end, T relativePrecision, T minimalStep, T maximalStep) {
  assert(relativePrecision > static_cast<T>(0.f) && minimalStep >= static_cast<T>(0.f) && maximalStep >= minimalStep);

  constexpr T overflow = Float<T>::Max();
  constexpr T underflow = Float<T>::Min();

  if (start == static_cast<T>(INFINITY)) {
    start = overflow;
  } else if (start == static_cast<T>(-INFINITY)) {
    start = -overflow;
  }
  if (end == static_cast<T>(INFINITY)) {
    end = overflow;
  } else if (end == static_cast<T>(-INFINITY)) {
    end = -overflow;
  }
  if (minimalStep == 0) {
    minimalStep = underflow;
  }

  if (start * end >= static_cast<T>(0.f)) {
    return NextPointOfInterestHelper(evaluation, context, auxiliary, search, start, end, relativePrecision, minimalStep, maximalStep);
  }

  /* By design, NextPointOfInterestHelper only works on intervals that do not
   * contain 0. If start and end are of different signs, we have to cut the
   * interval in three: negative, around 0, and positive. */

  assert(start * end < static_cast<T>(0.f));
  Coordinate2D<T> result = NextPointOfInterestHelper(evaluation, context, auxiliary, search, start, static_cast<T>(0.f), relativePrecision, minimalStep, maximalStep);
  if (std::isfinite(result.x1())) {
    /* Althoug this method can return NaN when there is no solution, here we
     * only return if a solution was found, as there are two other intervals to
     * check otherwise. */
    return result;
  }
  constexpr T marginAroundZero = static_cast<T>(1e-3);
  /* We deviate slightly from zero, as many common functions with an
   * interesting behaviour around zero are undefined for x=0. */
  T fakeZero = (sizeof(T) == sizeof(float) ? FLT_EPSILON : DBL_EPSILON) * static_cast<T>(start < end ? 1.f : -1.f);
  result = search(-marginAroundZero, fakeZero, marginAroundZero, evaluation(-marginAroundZero, context, auxiliary), evaluation(fakeZero, context, auxiliary), evaluation(marginAroundZero, context, auxiliary), evaluation, context, auxiliary);
  if (std::isfinite(result.x1())) {
    return result;
  }
  return NextPointOfInterestHelper(evaluation, context, auxiliary, search, static_cast<T>(0.f), end, relativePrecision, minimalStep, maximalStep);
}

template<typename T>
Coordinate2D<T> SolverHelper<T>::NextPointOfInterestHelper(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, BracketSearch search, T start, T end, T relativePrecision, T minimalStep, T maximalStep) {
  assert(start * end >= static_cast<T>(0.f));
  assert(relativePrecision > static_cast<T>(0.f));

  T h = std::fabs(start) < std::fabs(end) ? relativePrecision : -relativePrecision;
  T m, M;
  if (start < end) {
    m = minimalStep;
    M = maximalStep;
  } else {
    m = -minimalStep;
    M = -maximalStep;
  }

  T x1 = start, x2 = Step(start, h, m, M);
  T x3 = Step(x2, h, m, M);
  T y1 = evaluation(x1, context, auxiliary), y2 = evaluation(x2, context, auxiliary), y3 = evaluation(x3, context, auxiliary);

  Coordinate2D<T> result(NAN, NAN);

  while (!std::isfinite(result.x1()) && std::isfinite(x1) && (x1 < end) == (start < end)) {
    result = search(x1, x2, x3, y1, y2, y3, evaluation, context, auxiliary);

    x1 = x2;
    y1 = y2;
    x2 = x3;
    y2 = y3;
    x3 = Step(x2, h, m, M);
    y3 = evaluation(x3, context, auxiliary);
  }

  return result;
}

template<typename T>
T SolverHelper<T>::Step(T x, T growthSpeed, T minimalStep, T maximalStep) {
  assert(std::fabs(minimalStep) <= std::fabs(maximalStep));
  T acceleration = std::fmax(
      std::fabs(std::log10(std::fabs(x)) - static_cast<T>(1.f)) - static_cast<T>(2.f),
      static_cast<T>(0.f));
  /* For growthSpeed < 0 and x large enough, step < -x. The new value of x
   * would change sign, which is prohibited. We thus bound the step so that x
   * cannot lose more than one order of magnitude. */
  constexpr T maxLossOfMagnitude = static_cast<T>(-0.9f);
  T step = x * std::fmax(growthSpeed * (static_cast<T>(1.f) + acceleration), maxLossOfMagnitude);
  if (!std::isfinite(step) || std::fabs(minimalStep) > std::fabs(step)) {
    step = minimalStep;
  } else if (std::fabs(maximalStep) < std::fabs(step)) {
    step = maximalStep;
  }
  T res = x + step;
  assert(minimalStep > 0 ? res > x : res < x);
  return res;
}

template<typename T>
bool SolverHelper<T>::RootExistsOnInterval(T fa, T fb, T fc) {
  return ((fb == static_cast<T>(0.f) && fa * fc < static_cast<T>(0.f))
       || fb * fc < static_cast<T>(0.f));
}

/* FIXME Temporarily keep SolverHelper until Zoom is refactored to use Solver. */
template Coordinate2D<float> SolverHelper<float>::NextPointOfInterest(ValueAtAbscissa evaluation, Context * context, const void * auxiliary, BracketSearch search, float start, float end, float relativePrecision, float minimalStep, float maximalStep);
template bool SolverHelper<float>::RootExistsOnInterval(float fa, float fb, float fc);
#endif

}
