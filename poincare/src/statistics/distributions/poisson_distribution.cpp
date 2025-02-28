#include <assert.h>
#include <omg/float.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/src/statistics/distributions/poisson_distribution.h>
#include <poincare/statistics/distribution.h>

#include <cmath>

namespace Poincare::Internal::PoissonDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const T* parameters) {
  if (x < 0) {
    return NAN;
  }
  const T lambda = parameters[0];
  T lResult = -lambda + std::floor(x) * std::log(lambda) -
              std::lgamma(std::floor(x) + 1);
  return std::exp(lResult);
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters) {
  constexpr T precision = OMG::Float::Epsilon<T>();
  if (std::abs(probability) < precision) {
    return NAN;
  }
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    return INFINITY;
  }
  T proba = probability;
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        const T* params = static_cast<const T*>(auxiliary);
        return EvaluateAtAbscissa(x, params);
      },
      parameters);
}

template float EvaluateAtAbscissa<float>(float, const float*);
template double EvaluateAtAbscissa<double>(double, const double*);
template float CumulativeDistributiveInverseForProbability<float>(float,
                                                                  const float*);
template double CumulativeDistributiveInverseForProbability<double>(
    double, const double*);

}  // namespace Poincare::Internal::PoissonDistribution
