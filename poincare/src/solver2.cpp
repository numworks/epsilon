#include <poincare/solver2.h>
#include <poincare/solver_algorithms.h>

namespace Poincare {

template<typename T>
Solver2<T>::Solver2(ExpressionNode::ComputationContext * context, T xStart, T xEnd, const char * unknown, T precision) :
  m_xStart(xStart),
  m_xEnd(xEnd),
  m_yResult(k_NAN),
  m_precision(precision),
  m_context(context),
  m_unknown(unknown),
  m_lastInterest(Interest::None)
{
  assert(context);
  assert(unknown && unknown[0] != '\0');
}

template<typename T>
Solver2<T>::Solver2(const Solver2<T> * other) :
  Solver2(other->m_context, other->m_xStart, other->m_xEnd, other->m_unknown, other->m_precision)
{}

template<typename T>
Coordinate2D<T> Solver2<T>::next(FunctionEvaluation f, const void * aux, BracketTest test, HoneResult hone) {
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
Coordinate2D<T> Solver2<T>::next(Expression e, BracketTest test, HoneResult hone) {
  FunctionEvaluationParameters parameters = { .context = m_context->context(), .unknown = m_unknown, .expression = e, .complexFormat = m_context->complexFormat(), .angleUnit = m_context->angleUnit() };
  FunctionEvaluation f = [](T x, const void * aux) {
    const FunctionEvaluationParameters * p = reinterpret_cast<const FunctionEvaluationParameters *>(aux);
    return p->expression.approximateWithValueForSymbol(p->unknown, x, p->context, p->complexFormat, p->angleUnit);
  };

  return next(f, &parameters, test, hone);
}

template<typename T>
Coordinate2D<T> Solver2<T>::nextRoot(Expression e) {
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
    return nextRootNumeric(e);
  }
}

template<typename T>
Coordinate2D<T> Solver2<T>::nextMinimum(Expression e) {
  return next(e, MinimumInBracket, SolverAlgorithms::BrentMinimum);
}

template<typename T>
T Solver2<T>::nextX(T x, T direction) const {
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
  constexpr T minimalAbsoluteStep = static_cast<T>(1e-3);
  constexpr T minimalNumberOfSteps = static_cast<T>(100.);
  constexpr T baseGrowthSpeed = static_cast<T>(1.1);
  static_assert(baseGrowthSpeed > static_cast<T>(1.), "Growth speed must be greater than 1");
  constexpr T maximalGrowthSpeed = static_cast<T>(10.);
  constexpr T growthSpeedAcceleration = static_cast<T>(1e-2);
  /* Increase density between 0.1 and 100 */
  constexpr T lowerTypicalMagnitude = static_cast<T>(-1.);
  constexpr T upperTypicalMagnitude = static_cast<T>(2.);

  T maximalStep = std::fabs(m_xEnd - m_xStart) / minimalNumberOfSteps;
  T stepSign = x < direction ? static_cast<T>(1.) : static_cast<T>(-1.);

  T magnitude = std::log(std::fabs(x));
  if (!std::isfinite(magnitude)) {
    return x + stepSign * minimalAbsoluteStep;
  }
  T ratio;
  if (lowerTypicalMagnitude < magnitude && magnitude < upperTypicalMagnitude) {
    ratio = baseGrowthSpeed;
  } else {
    T exponent = magnitude < lowerTypicalMagnitude ? magnitude - lowerTypicalMagnitude : upperTypicalMagnitude - magnitude;
    ratio = maximalGrowthSpeed - (maximalGrowthSpeed - baseGrowthSpeed) * std::exp(growthSpeedAcceleration * std::pow(exponent, 3.));
  }
  T x2 = (x < direction) == (x < k_zero) ? x / ratio : x * ratio;
  if (std::fabs(x - x2) < minimalAbsoluteStep) {
    x2 = x + stepSign * minimalAbsoluteStep;
  } else if (std::fabs(x - x2) > maximalStep) {
    x2 = x + stepSign * maximalStep;
  }
  assert((x < x2) == (x < direction));
  assert(x2 != x);
  return x2;
}

template<typename T>
Coordinate2D<T> Solver2<T>::nextPossibleRootInChild(Expression e, int childIndex) const {
  Solver2<T> solver(this);
  Expression child = e.childAtIndex(childIndex);
  T xRoot = solver.nextRoot(child).x1();
  while (std::isfinite(xRoot)) {
    if (std::fabs(e.approximateWithValueForSymbol<T>(m_unknown, xRoot, m_context->context(), m_context->complexFormat(), m_context->angleUnit())) < NullTolerance(m_precision)) {
      return Coordinate2D<T>(xRoot, k_zero);
    }
    xRoot = solver.nextRoot(child).x1();
  }
  return Coordinate2D<T>(k_NAN, k_NAN);
}

template<typename T>
Coordinate2D<T> Solver2<T>::nextRootInMultiplication(Expression e) const {
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
Coordinate2D<T> Solver2<T>::nextRootNumeric(Expression e) {
  /* Even roots (e.g. x^2 for x = 0) cannot be found by searching for
   * changes in sign. We also need to look for extrema, and check if they
   * are roots. */
  BracketTest test = [](T a, T b, T c) {
    Interest root = RootInBracket(a, b, c);
    if (root != Interest::None) {
      return root;
    }
    /* FIXME Check the sign of a,b and c. A minimum can only be a root if b is
     * positive. */
    Interest extremum = MinimumInBracket(a, b, c);
    return extremum == Interest::None ? MaximumInBracket(a, b, c) : extremum;
  };

  HoneResult hone = [](FunctionEvaluation f, const void * aux, T xMin, T xMax, Interest interest, T precision) {
    if (interest == Interest::Root) {
      return SolverAlgorithms::BrentRoot(f, aux, xMin, xMax, interest, precision);
    }
    Coordinate2D<T> res;
    if (interest == Interest::LocalMinimum) {
      res = SolverAlgorithms::BrentMinimum(f, aux, xMin, xMax, interest, precision);
    } else {
      assert(interest == Interest::LocalMaximum);
      const void * pack[] = {&f, aux};
      FunctionEvaluation minusF = [](T x, const void * aux) {
        const void * const * param = reinterpret_cast<const void * const *>(aux);
        const FunctionEvaluation * f = reinterpret_cast<const FunctionEvaluation *>(param[0]);
        return -(*f)(x, param[1]);
      };
      res = SolverAlgorithms::BrentMinimum(minusF, pack, xMin, xMax, interest, precision);
    }
    if (std::isfinite(res.x1()) && std::fabs(res.x2()) < NullTolerance(precision)) {
      return res;
    }
    return Coordinate2D<T>(k_NAN, k_NAN);
  };

  Coordinate2D<T> res = next(e, test, hone);
  if (lastInterest() != Interest::None) {
    m_lastInterest = Interest::Root;
  }
  return res;
}


template<typename T>
void Solver2<T>::registerSolution(Coordinate2D<T> solution, Interest interest) {
  if (std::isfinite(solution.x1())) {
    m_xStart = solution.x1();
    m_yResult = solution.x2();
    m_lastInterest = interest;
  } else {
    m_lastInterest = Interest::None;
  }
}

// Explicit template instanciations

template Coordinate2D<double> Solver2<double>::nextRoot(Expression e);
template Coordinate2D<double> Solver2<double>::nextMinimum(Expression e);
template Coordinate2D<double> Solver2<double>::nextRootNumeric(Expression e);

}
