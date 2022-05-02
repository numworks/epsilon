#include <poincare/poisson_distribution.h>
#include <poincare/float.h>
#include <poincare/rational.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <poincare/solver.h>
#include <poincare/distribution.h>
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Poincare {

template<typename T>
T PoissonDistribution::EvaluateAtAbscissa(T x, T lambda) {
  if (std::isnan(x) || std::isinf(x) || !LambdaIsOK(lambda)){
    return NAN;
  }
  if (x < 0) {
    return NAN;
  }
  T lResult = -lambda + std::floor(x) * std::log(lambda) - std::lgamma(std::floor(x) + 1);
  return std::exp(lResult);
}

template<typename T>
T PoissonDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T lambda) {
  if (!LambdaIsOK(lambda) || std::isnan(x) || std::isinf(x)) {
    return NAN;
  }
  if (std::isinf(x)) {
    return x > static_cast<T>(0.0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }

  if (x < static_cast<T>(0.0)) {
    return static_cast<T>(0.0);
  }
  const void * pack[1] = { &lambda };
  return Solver::CumulativeDistributiveFunctionForNDefinedFunction<T>(x,
        [](double k, Poincare::Context * context, const void * auxiliary) {
          const void * const * pack = static_cast<const void * const *>(auxiliary);
          double lambda = *static_cast<const T *>(pack[0]);
          return  (double)PoissonDistribution::EvaluateAtAbscissa<T>(k, lambda);
        },
        nullptr, pack);
}

template<typename T>
T PoissonDistribution::CumulativeDistributiveInverseForProbability(T probability, T lambda) {
  if (!LambdaIsOK(lambda) || std::isnan(probability) || std::isinf(probability) || probability < static_cast<T>(0.0) || probability > static_cast<T>(1.0)) {
    return NAN;
  }
  constexpr T precision = Float<T>::Epsilon();
  if (std::abs(probability) < precision) {
    return NAN;
  }
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    return INFINITY;
  }
  T proba = probability;
  const void * pack[1] = { &lambda };
  return Solver::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](double x, Context * context, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        double lambda = *static_cast<const T *>(pack[0]);
        return (double)PoissonDistribution::EvaluateAtAbscissa<T>(x, lambda);
      }, nullptr, pack);
}

template<typename T>
bool PoissonDistribution::LambdaIsOK(T lambda) {
  return !std::isnan(lambda) && !std::isinf(lambda)
    && (lambda == static_cast<int>(lambda)) && (lambda > static_cast<T>(0.0))
    && (lambda <= static_cast<T>(999.0)); // usefull ?
}

bool PoissonDistribution::ExpressionLambdaIsOK(bool * result, const Expression & lambda, Context * context) {
  assert(result != nullptr);
  if (lambda.deepIsMatrix(context)) {
    *result = false;
    return true;
  }

  if (lambda.isUndefined() || Expression::IsInfinity(lambda, context)) {
    *result = false;
    return true;
  }
  if (!lambda.isReal(context)) {
    // We cannot check that lambda is real
    return false;
  }

  if (lambda.type() != ExpressionNode::Type::Rational) {
    // We cannot check that lambda is positive
    return false;
  }

  {
    const Rational rationalLambda = static_cast<const Rational &>(lambda);
    if (rationalLambda.isNegative() || rationalLambda.isZero()) {
      // lambda is negative or null
      *result = false;
      return true;
    }
  }

  *result = true;
  return true;
}


template float PoissonDistribution::EvaluateAtAbscissa<float>(float, float);
template double PoissonDistribution::EvaluateAtAbscissa<double>(double, double);
template float PoissonDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float);
template double PoissonDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double);
template float PoissonDistribution::CumulativeDistributiveInverseForProbability<float>(float, float);
template double PoissonDistribution::CumulativeDistributiveInverseForProbability<double>(double, double);
template bool PoissonDistribution::LambdaIsOK(float);
template bool PoissonDistribution::LambdaIsOK(double);

}
