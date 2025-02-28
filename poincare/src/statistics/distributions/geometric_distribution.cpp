#include <assert.h>
#include <omg/float.h>
#include <poincare/src/solver/regularized_incomplete_beta_function.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/src/statistics/distributions/geometric_distribution.h>
#include <poincare/statistics/distribution.h>

#include <cmath>

namespace Poincare::Internal::GeometricDistribution {

template <typename T>
T EvaluateAtAbscissa(T x, const T* params) {
  if (std::isinf(x)) {
    return NAN;
  }
  constexpr T castedOne = static_cast<T>(1.0);
  if (x < castedOne) {
    return static_cast<T>(0.0);
  }
  const T p = params[0];
  if (p == castedOne) {
    return x == castedOne ? castedOne : static_cast<T>(0.0);
  }
  // The result is p * (1-p)^{k-1}
  T lResult = (std::floor(x) - castedOne) * std::log(castedOne - p);
  return p * std::exp(lResult);
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability, const T* params) {
  constexpr T precision = OMG::Float::Epsilon<T>();
  if (std::abs(probability) < precision) {
    return NAN;
  }
  const T p = params[0];
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    if (std::abs(p - static_cast<T>(1.0)) < precision) {
      return static_cast<T>(1.0);
    }
    return INFINITY;
  }
  T proba = probability;
  /* It works even if G(p) is defined on N* and not N because G(0) returns 0 and
   * not undef */
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        return EvaluateAtAbscissa(x, static_cast<const T*>(auxiliary));
      },
      params);
}

template float EvaluateAtAbscissa<float>(float, const float*);
template double EvaluateAtAbscissa<double>(double, const double*);
template float CumulativeDistributiveInverseForProbability<float>(float,
                                                                  const float*);
template double CumulativeDistributiveInverseForProbability<double>(
    double, const double*);

}  // namespace Poincare::Internal::GeometricDistribution
