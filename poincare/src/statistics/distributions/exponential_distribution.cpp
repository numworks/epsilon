#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/statistics/distributions/exponential_distribution.h>

#include <cmath>

namespace Poincare::Internal::ExponentialDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const T* params) {
  if (x < static_cast<T>(0.0)) {
    return NAN;
  }
  const T lambda = params[0];
  return lambda * std::exp(-lambda * x);
}

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(T x, const T* params) {
  if (x < 0.0) {
    return static_cast<T>(0.0);
  }
  const T lambda = params[0];
  return static_cast<T>(1.0) - std::exp((-lambda * x));
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability, const T* params) {
  const T lambda = params[0];
  return -std::log(1.0 - probability) / lambda;
}

// Specialisations
template float EvaluateAtAbscissa<float>(float, const float*);
template double EvaluateAtAbscissa<double>(double, const double*);
template float CumulativeDistributiveFunctionAtAbscissa<float>(float,
                                                               const float*);
template double CumulativeDistributiveFunctionAtAbscissa<double>(double,
                                                                 const double*);
template float CumulativeDistributiveInverseForProbability<float>(float,
                                                                  const float*);
template double CumulativeDistributiveInverseForProbability<double>(
    double, const double*);

}  // namespace Poincare::Internal::ExponentialDistribution
