#include <omg/float.h>
#include <poincare/src/equation_solver/equation_solver.h>
#include <poincare/src/memory/n_ary.h>

#include <algorithm>

#include "approximation.h"
#include "k_tree.h"
#include "parametric.h"
#include "simplification.h"
#include "variables.h"

namespace Poincare::Internal::Approximation::Private {

template <typename T>
bool IsErrorNegligible(DetailedResult<T> result, T absoluteErrorThreshold,
                       T relativeErrorThreshold) {
  return (result.absoluteError < absoluteErrorThreshold ||
          result.absoluteError <
              relativeErrorThreshold * std::fabs(result.integral));
}

template <typename T>
bool DetailedResultIsValid(DetailedResult<T> result) {
  constexpr T absoluteErrorThresholdForValidity = 0.1;
  constexpr T relativeErrorThresholdForValidity = 1e-4;

  return std::isfinite(result.integral) &&
         IsErrorNegligible(result, absoluteErrorThresholdForValidity,
                           relativeErrorThresholdForValidity);
}

template <typename T>
T Approximator::approximateIntegral(const Tree* integral) {
  const Tree* lowerBound = integral->child(Parametric::k_lowerBoundIndex);
  const Tree* upperBound = lowerBound->nextTree();
  T a = privateToFloat<T>(lowerBound);
  T b = privateToFloat<T>(upperBound);
  // a and b are allowed to be infinite but not NaN
  if (std::isnan(a) || std::isnan(b)) {
    return NAN;
  }

  /* Detect singularities inside the bounds (not all singularities can be
   * detected). Split the integral at these singularity points */
  Tree* splitIntegral = splitAtSingularities(integral, a, b);
  assert(splitIntegral->isList() && splitIntegral->numberOfChildren() >= 1);

  T result = 0.0;
  for (const Tree* childIntegral : splitIntegral->children()) {
    result += approximateIntegralInternal<T>(childIntegral);
  }
  splitIntegral->removeTree();
  return result;
}

template <typename T>
Tree* Approximator::splitAtSingularities(const Tree* integral, T a, T b) {
  const Tree* lowerBound = integral->child(Parametric::k_lowerBoundIndex);
  const Tree* upperBound = lowerBound->nextTree();
  const Tree* integrand = upperBound->nextTree();

  TreeRef result = SharedTreeStack->pushList(0);

  EquationSolver::SingularitySolverResult singularities =
      EquationSolver::ExactSolveFindSingularities(integrand, m_parameters);

  int i = 0;
  // TODO: NAry::RemoveIf
  while (i < singularities.exactSolutionList->numberOfChildren()) {
    T value = privateToFloat<T>(singularities.exactSolutionList->child(i));
    if (value <= a || value >= b) {
      NAry::RemoveChildAtIndex(singularities.exactSolutionList, i);
    } else {
      i++;
    }
  }

  if (singularities.exactSolutionList->numberOfChildren() == 0) {
    singularities.exactSolutionList->cloneTreeOverTree(KUndef);
  }

  if (!singularities.exactSolutionList->isUndefined()) {
    assert(singularities.exactSolutionList->isList() &&
           singularities.exactSolutionList->numberOfChildren() >= 1);
    // TODO:
    // - if some of them are, split the integral
    NAry::AddChild(result, integral->cloneTree());
  } else {
    NAry::AddChild(result, integral->cloneTree());
  }
  singularities.exactSolutionList->removeTree();
  return result;
}

template <typename T>
T Approximator::approximateIntegralInternal(const Tree* integral) {
  const Tree* lowerBound = integral->child(Parametric::k_lowerBoundIndex);
  const Tree* upperBound = lowerBound->nextTree();
  const Tree* integrand = upperBound->nextTree();
  T a = privateToFloat<T>(lowerBound);
  T b = privateToFloat<T>(upperBound);

  // Handle singularities at the bounds
  bool notFiniteInA = !std::isfinite(toLocalContext(integrand, a));
  bool notFiniteInB = !std::isfinite(toLocalContext(integrand, b));
  if (notFiniteInA || notFiniteInB) {
    T value = approximateIntegralSingularityAtBoundsMethod<T>(
        integral, a, b, notFiniteInA, notFiniteInB);
    if (!std::isnan(value)) {
      return value;
    }
  }

  /* Even for integrals with singularities at the bounds, sometimes the method
   * for singularities at bounds (tanh-sinh quadrature) fails but the generic
   * method (adaptive Kronrod-Gauss quadrature) succeeds */
  return approximateIntegralGenericMethod(integral, a, b);
}

template <typename T>
T Approximator::approximateIntegralSingularityAtBoundsMethod(
    const Tree* integral, T a, T b, bool notFiniteInA, bool notFiniteInB) {
  /* When we have a singularity at a bound, we want to evaluate the integrand
   * really close to the bound since the area there is non-negligible.  If the
   * bound is non-null, say 1/sqrt(1-x) near 1, the closest point from 1 where
   * the integrand can be evaluated is 1-1e-15. However by using reduction to
   * simplify the expression near one 1/sqrt(1-(1-dx)) = 1/sqrt(dx) we can
   * evaluate the integrand really close to 1 (about 1-1e-300). */
  Private::AlternativeIntegrand alternativeIntegrand = {.a = a, .b = b};
  if (notFiniteInA && a != 0 && integral->isIntegralWithAlternatives()) {
    alternativeIntegrand.integrandNearA = integral->child(4);
  }
  if (notFiniteInB && b != 0 && integral->isIntegralWithAlternatives()) {
    alternativeIntegrand.integrandNearB = integral->child(5);
  }
  /* We are using 4 levels of refinement which means ≈ 64 integrand
   * evaluations = 2 (R- and R+) * 2^4 (ticks/unit) * 4 (typical decay on the
   * examples) It could be increased but precision is likely lost somewhere
   * else in hard examples. */
  const Tree* integrand = integral->child(Parametric::k_integrandIndex);
  Private::DetailedResult<T> detailedResult =
      tanhSinhQuadrature<T>(integrand, 4, alternativeIntegrand);
  // Arbitrary value to have the best choice of quadrature on the examples
  constexpr T insufficientPrecision = 0.001;
  if (!std::isnan(detailedResult.integral) &&
      detailedResult.absoluteError < insufficientPrecision) {
    return detailedResult.integral;
  }
  return NAN;
}

template <typename T>
T Approximator::approximateIntegralGenericMethod(const Tree* integral, T a,
                                                 T b) {
  const Tree* integrand = integral->child(Parametric::k_integrandIndex);

  /* Choose the right substitution to use in Gauss-Kronrod.
   * We are using a (0,inf) -> (-1,1) like substitution when a bound is above
   * these thresholds to deal with big but finite bounds */
  constexpr T leftOpenThreshold = -1000.0;
  constexpr T rightOpenThreshold = 1000.0;
  T start, end, scale = 1.0;
  if (a > b) {
    scale = -1.0;
    std::swap(a, b);
  }
  Private::Substitution<T> substitution;
  substitution.originA = a;
  substitution.originB = b;
  if (a < leftOpenThreshold) {
    if (b > rightOpenThreshold) {
      substitution.type = Private::Substitution<T>::Type::RealLine;
      // Two solutions but we need the one with the sign of a (resp. b)
      start = std::isfinite(a)
                  ? ((std::sqrt(4 * a * a + 1.0) - 1.0) / (2.0 * a))
                  : -1.0;
      end = std::isfinite(b) ? ((std::sqrt(4 * b * b + 1.0) - 1.0) / (2.0 * b))
                             : 1.0;
    } else {
      substitution.type = Private::Substitution<T>::Type::LeftOpen;
      start = std::isfinite(a) ? (a - b + 1.0) / (b - a + 1.0) : -1.0;
      end = 1.0;
      scale *= 2.0;
    }
  } else {
    if (b > rightOpenThreshold) {
      substitution.type = Private::Substitution<T>::Type::RightOpen;
      start = std::isfinite(b) ? (a - b + 1.0) / (b - a + 1.0) : -1.0;
      end = 1.0;
      scale *= 2.0;
    } else {
      substitution.type = Private::Substitution<T>::Type::None;
      start = a;
      end = b;
    }
  }

  Private::DetailedResult<T> detailedResult =
      adaptiveQuadrature<T>(integrand, start, end, substitution);
  return DetailedResultIsValid(detailedResult) ? scale * detailedResult.integral
                                               : NAN;
}

template <typename T>
T Approximator::evaluateIntegrand(const Tree* integrand, T x,
                                  Private::Substitution<T> substitution) {
  switch (substitution.type) {
    case Private::Substitution<T>::Type::None:
      return Approximator::toLocalContext(integrand, x);
    case Private::Substitution<T>::Type::LeftOpen: {
      T z = 1.0 / (x + 1.0);
      T arg = substitution.originB - (2.0 * z - 1.0);
      return Approximator::toLocalContext(integrand, arg) * z * z;
    }
    case Private::Substitution<T>::Type::RightOpen: {
      T z = 1.0 / (x + 1);
      T arg = 2.0 * z + substitution.originA - 1.0;
      return Approximator::toLocalContext(integrand, arg) * z * z;
    }
    default: {
      assert(substitution.type == Private::Substitution<T>::Type::RealLine);
      T x2 = x * x;
      T inv = 1.0 / (1.0 - x2);
      T w = (1.0 + x2) * inv * inv;
      T arg = x * inv;
      return Approximator::toLocalContext(integrand, arg) * w;
    }
  }
}

template <typename T>
T Approximator::evaluateIntegrandNearBound(
    const Tree* integrand, T x, T xc,
    Private::AlternativeIntegrand alternativeIntegrand) {
  T scale = (alternativeIntegrand.b - alternativeIntegrand.a) / 2.0;
  T arg = xc * scale;
  if (x < 0) {
    if (alternativeIntegrand.integrandNearA) {
      return toLocalContext(alternativeIntegrand.integrandNearA, arg) * scale;
    }
    arg = arg + alternativeIntegrand.a;
  } else {
    if (alternativeIntegrand.integrandNearB) {
      return toLocalContext(alternativeIntegrand.integrandNearB, -arg) * scale;
    }
    arg = alternativeIntegrand.b - arg;
  }
  return toLocalContext(integrand, arg) * scale;
}

/* Tanh-Sinh quadrature
 * cf https://www.davidhbailey.com/dhbpapers/dhb-tanh-sinh.pdf */
template <typename T>
Private::DetailedResult<T> Approximator::tanhSinhQuadrature(
    const Tree* integrand, int level,
    Private::AlternativeIntegrand alternativeIntegrand) {
  T h = 2.0;
  // j=0
  T result = M_PI_2 * evaluateIntegrandNearBound(integrand, 0.0, 1.0,
                                                 alternativeIntegrand);
  int j = 1;
  T sn2 = 0, sn1 = 0;
  T maxWjFj = 0;
  for (int k = 0; k < level; k++) {
    sn2 = sn1;
    sn1 = result * h;
    h = h / 2.0;
    j = 1;
    bool leftOk = true;
    bool rightOk = true;
    while (leftOk || rightOk) {
      T sinh = M_PI_2 * std::sinh(h * j);
      T cs = std::cosh(sinh);
      T weight = M_PI_2 * std::cosh(h * j) / (cs * cs);
      T abscissa = std::tanh(sinh);
      T distanceToBound = 1.0 / (std::exp(sinh) * std::cosh(sinh));
      if (leftOk) {
        T leftValue = evaluateIntegrandNearBound(
            integrand, -abscissa, distanceToBound, alternativeIntegrand);
        if (!std::isfinite(leftValue)) {
          leftOk = false;
        } else {
          maxWjFj = std::max(maxWjFj, std::abs(weight * leftValue));
          result += weight * leftValue;
        }
        /* Criterion used in boost:
         * abs(y * weights) > abs(L1_I0 * tail_tolerance)
         * but L1_IO is abs(pi/2 * f(0)) before the first row and
         * tail_tolerance = tolerance^2 */
        if (std::abs(weight * leftValue) < OMG::Float::EpsilonLax<T>())
          leftOk = false;
      }
      if (rightOk) {
        T rightValue = evaluateIntegrandNearBound(
            integrand, abscissa, distanceToBound, alternativeIntegrand);
        if (!std::isfinite(rightValue)) {
          rightOk = false;
        } else {
          maxWjFj = std::max(maxWjFj, std::abs(weight * rightValue));
          result += weight * rightValue;
        }
        if (std::abs(weight * rightValue) < OMG::Float::EpsilonLax<T>())
          rightOk = false;
      }
      // computing only odd ticks after the first level
      if (k) {
        j += 2;
      } else {
        j += 1;
      }
    }
  }
  T error;
  T sn = h * result;
  if (sn == sn1) {
    error = 0.0;
  } else {
    /* We need to have an estimation of the precision to be able to know that
     * 1/x is nonintegrable between 0 and 1 for instance */
    T d1 = std::log10(std::abs(sn - sn1));
    T d2 = std::log10(std::abs(sn1 - sn2));
    T d3 = std::log10(maxWjFj) - 15;
    T d = std::max({d1 * d1 / d2, 2 * d1, d3});
    error = std::pow(static_cast<T>(10.0), d);
  }
  Private::DetailedResult<T> detailedResult;
  detailedResult.integral = sn;
  detailedResult.absoluteError = error;
  return detailedResult;
}

template <typename T>
Private::DetailedResult<T> Approximator::kronrodGaussQuadrature(
    const Tree* integrand, T a, T b, Private::Substitution<T> substitution) {
  constexpr T max = sizeof(T) == sizeof(double) ? DBL_MAX : FLT_MAX;
  /* We here use Kronrod-Legendre quadrature with n = 21
   * The abscissa and weights are taken from QUADPACK library. */

  // Abscissae for the gauss (odd weights) and kronrod rules (all weights)
  constexpr T x[11] = {
      0.995657163025808080735527280689003, 0.973906528517171720077964012084452,
      0.930157491355708226001207180059508, 0.865063366688984510732096688423493,
      0.780817726586416897063717578345042, 0.679409568299024406234327365114874,
      0.562757134668604683339000099272694, 0.433395394129247190799265943165784,
      0.294392862701460198131126603103866, 0.148874338981631210884826001129720,
      0.000000000000000000000000000000000};

  // Weights for the gauss integral
  constexpr T wGauss[5] = {
      0.066671344308688137593568809893332, 0.149451349150580593145776339657697,
      0.219086362515982043995534934228163, 0.269266719309996355091226921569469,
      0.295524224714752870173892994651338};

  // Weights for the kronrod rule
  constexpr T wKronrod[11] = {
      0.011694638867371874278064396062192, 0.032558162307964727478818972459390,
      0.054755896574351996031381300244580, 0.075039674810919952767043140916190,
      0.093125454583697605535065465083366, 0.109387158802297641899210590325805,
      0.123491976262065851077958109831074, 0.134709217311473325928054001771707,
      0.142775938577060080797094273138717, 0.147739104901338491374841515972068,
      0.149445554002916905664936468389821};

  T fv1[10];
  T fv2[10];

  T center = static_cast<T>(0.5) * (a + b);
  T halfLength = static_cast<T>(0.5) * (b - a);
  T absHalfLength = std::fabs(halfLength);

  Private::DetailedResult<T> errorResult;
  errorResult.integral = NAN;
  errorResult.absoluteError = 0;

  T gaussIntegral = 0;
  T fCenter = evaluateIntegrand(integrand, center, substitution);
  if (!std::isfinite(fCenter)) {
    return errorResult;
  }
  T kronrodIntegral = wKronrod[10] * fCenter;
  T absKronrodIntegral = std::fabs(kronrodIntegral);
  for (int j = 0; j < 10; j++) {
    T xDelta = halfLength * x[j];
    T fval1 = evaluateIntegrand(integrand, center - xDelta, substitution);
    if (!std::isfinite(fval1)) {
      return errorResult;
    }
    T fval2 = evaluateIntegrand(integrand, center + xDelta, substitution);
    if (!std::isfinite(fval2)) {
      return errorResult;
    }
    fv1[j] = fval1;
    fv2[j] = fval2;
    T fsum = fval1 + fval2;
    if (j % 2 == 1) {
      gaussIntegral += wGauss[j / 2] * fsum;
    }
    kronrodIntegral += wKronrod[j] * fsum;
    absKronrodIntegral += wKronrod[j] * (std::fabs(fval1) + std::fabs(fval2));
  }

  T halfKronrodIntegral = static_cast<T>(0.5) * kronrodIntegral;
  T kronrodIntegralDifference =
      wKronrod[10] * std::fabs(fCenter - halfKronrodIntegral);
  for (int j = 0; j < 10; j++) {
    kronrodIntegralDifference +=
        wKronrod[j] * (std::fabs(fv1[j] - halfKronrodIntegral) +
                       std::fabs(fv2[j] - halfKronrodIntegral));
  }
  T integral = kronrodIntegral * halfLength;
  absKronrodIntegral = absKronrodIntegral * absHalfLength;
  kronrodIntegralDifference = kronrodIntegralDifference * absHalfLength;
  T absError = std::fabs((kronrodIntegral - gaussIntegral) * halfLength);
  if (kronrodIntegralDifference != 0 && absError != 0) {
    T errorCoefficient =
        std::pow(static_cast<T>(200 * absError / kronrodIntegralDifference),
                 static_cast<T>(1.5));
    absError = 1 > errorCoefficient
                   ? kronrodIntegralDifference * errorCoefficient
                   : kronrodIntegralDifference;
  }
  if (absKronrodIntegral >
      max / (static_cast<T>(50.0) * OMG::Float::Epsilon<T>())) {
    T minError = OMG::Float::Epsilon<T>() * 50 * absKronrodIntegral;
    absError = absError > minError ? absError : minError;
  }
  Private::DetailedResult<T> result;
  result.integral = integral;
  result.absoluteError = absError;
  return result;
}

template <typename T>
Private::DetailedResult<T> Approximator::adaptiveQuadrature(
    const Tree* integrand, T a, T b, Private::Substitution<T> substitution) {
  Private::DetailedResult<T> quadKG =
      kronrodGaussQuadrature(integrand, a, b, substitution);

  /* Absolute and relative thresholds to consider that the quadrature
   * approximation have converged. This thresholds should be low enough to
   * ensure the integral is computed accurately.
   * The iterative algorithm first considers the whole integration interval.
   * There is a risk that the first quadratures miss some parts of the integral
   * that are "invisible" when the interpolation points are far from each other.
   * We must avoid those cases, in which the algorithm early exits with an
   * incorrect approximation of the integral value. */
  constexpr T absoluteErrorThreshold = 1e-12;
  constexpr T relativeErrorThreshold = OMG::Float::SqrtEpsilon<T>();

  int numberOfCalls = 1;
  return iterateAdaptiveQuadrature(
      integrand, quadKG, a, b, absoluteErrorThreshold, relativeErrorThreshold,
      substitution, 1, numberOfCalls);
}

template <typename T>
Private::DetailedResult<T> Approximator::iterateAdaptiveQuadrature(
    const Tree* integrand, Private::DetailedResult<T> quadKG, T a, T b,
    T absoluteErrorThreshold, T relativeErrorThreshold,
    Private::Substitution<T> substitution, int iterationDepth,
    int& iterationCount) {
  // Maximum number of interval splits for the iterative quadrature
  constexpr static int k_maxIterationDepth = 20;
  /* Maximum number of calls to iterateAdaptiveQuadrature, to avoid going very
   * deep everywhere on the interval, which would mean 2^20 calls.
   * Example: int(sin(10^7*x), 0, 1) */
  constexpr static int k_maxIterationCount = 5000;

  /* The Kronrod-Gauss method returns an error value together with the integral
   * approximation. Because it is impossible to know the exact value of the
   * integral, this error is computed by the difference between the integral
   * approximations obtained by two different methods, namely Kronrod and Gauss
   * quadratures. */
  /* TODO: we could add a minimum number of steps to force the integration
   * interval to be split into several intervals. This would prevent some wrong
   * early exits that can occur if the error is below the threshold, but because
   * both Kronrod and Gauss quadratures have too few interpolation points
   * compared to the interval length. */
  if (IsErrorNegligible(quadKG, absoluteErrorThreshold,
                        relativeErrorThreshold) ||
      iterationDepth > k_maxIterationDepth ||
      iterationCount > k_maxIterationCount) {
    return quadKG;
  }

  T m = (a + b) / 2;
  Private::DetailedResult<T> left =
      kronrodGaussQuadrature(integrand, a, m, substitution);
  Private::DetailedResult<T> right =
      kronrodGaussQuadrature(integrand, m, b, substitution);
  iterationCount += 1;

  /* Start by the side with the biggest error to reach maximumError faster if
   * it can be reached. */
  bool leftFirst = left.absoluteError >= right.absoluteError;
  for (int i = 0; i < 2; i++) {
    bool currentIsLeft = ((i == 0) == leftFirst);
    Private::DetailedResult<T>* current = currentIsLeft ? &left : &right;
    T lowerBound = currentIsLeft ? a : m;
    T upperBound = currentIsLeft ? m : b;

    /* The iterateAdaptiveQuadrature function is called recursively on the
     * half-interval with the biggest error. The number of remaining iterations
     * decreases with each recursive call. Because the error returned by the
     * kronrodGaussQuadrature method is proportional to the interval length, the
     * initial error thresholds are divided by two. */
    *current = iterateAdaptiveQuadrature(
        integrand, *current, lowerBound, upperBound, absoluteErrorThreshold / 2,
        relativeErrorThreshold / 2, substitution, iterationDepth + 1,
        iterationCount);
    if (!DetailedResultIsValid(*current)) {
      return {NAN, NAN};
    }
  }
  Private::DetailedResult<T> result = {
      .integral = left.integral + right.integral,
      .absoluteError = left.absoluteError + right.absoluteError};
  return result;
}

#if !POINCARE_NO_FLOAT_APPROXIMATION
template float Approximator::approximateIntegral(const Tree* integral);
#endif
template double Approximator::approximateIntegral(const Tree* integral);

}  // namespace Poincare::Internal::Approximation::Private
