#include <poincare/poisson_distribution.h>
#include <poincare/float.h>
#include <poincare/domain.h>
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
      [](T x, Context * context, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        T lambda = *static_cast<const T *>(pack[0]);
        return PoissonDistribution::EvaluateAtAbscissa(x, lambda);
      }, nullptr, pack);
}

template<typename T>
bool PoissonDistribution::LambdaIsOK(T lambda) {
  return !std::isnan(lambda) && !std::isinf(lambda)
    && (lambda > static_cast<T>(0.0))
    && (lambda <= static_cast<T>(999.0)); // usefull ?
}

bool PoissonDistribution::ExpressionLambdaIsOK(bool * result, const Expression & lambda, Context * context) {
  return Domain::ExpressionIsIn(result, lambda, Domain::Type::RPlusStar, context);
}


template float PoissonDistribution::EvaluateAtAbscissa<float>(float, float);
template double PoissonDistribution::EvaluateAtAbscissa<double>(double, double);
template float PoissonDistribution::CumulativeDistributiveInverseForProbability<float>(float, float);
template double PoissonDistribution::CumulativeDistributiveInverseForProbability<double>(double, double);
template bool PoissonDistribution::LambdaIsOK(float);
template bool PoissonDistribution::LambdaIsOK(double);

}
