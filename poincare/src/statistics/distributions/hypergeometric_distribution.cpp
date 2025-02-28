#include <assert.h>
#include <omg/float.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/solver/solver_algorithms.h>
#include <poincare/src/statistics/distributions/hypergeometric_distribution.h>

#include <cmath>

namespace Poincare::Internal::HypergeometricDistribution {

template <typename T>
T EvaluateAtAbscissa(T k, const T* parameters) {
  const T N = parameters[k_NIndex];
  const T K = parameters[k_KIndex];
  const T n = parameters[k_nIndex];

  if (!std::isfinite(k) || n > N || K > N ||
      (N - K == N && K != static_cast<T>(0.))) {
    /* `N - K == N` checks if K is negligible compared to N, to avoid precision
     * errors. */
    return NAN;
  }
  k = std::floor(k);
  if (k < 0 || k > std::min(n, K)) {
    return 0;
  }
  /* We don't want BinomialCoefficient to generalize the formula
   * (K - k) > (N - n) is equivalent to (n - k) > (N - K), but numerically it
   * can give different results. The first comparison is the "understandable"
   * one, the second comparison is the same but subtracts values with same
   * magnitude. */
  if (k > K || (n - k) > (N - K) || (K - k) > (N - n)) {
    return 0;
  }
  T result = Approximation::FloatBinomial(K, k) *
             Approximation::FloatBinomial(N - K, n - k) /
             Approximation::FloatBinomial(N, n);
  if (result < 0 || result > 1) {
    // Precision errors
    return NAN;
  }
  return result;
}

template <typename T>
T CumulativeDistributiveInverseForProbability(T probability,
                                              const T* parameters) {
  const T N = parameters[k_NIndex];
  const T K = parameters[k_KIndex];
  const T n = parameters[k_nIndex];

  constexpr T precision = OMG::Float::Epsilon<T>();
  if (probability < precision) {
    // We can have 0 successes only if there are enough failures
    if (n > N - K) {
      return 0;
    }
    return NAN;
  }
  if (1.0 - probability < precision) {
    return std::min(n, K);
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

}  // namespace Poincare::Internal::HypergeometricDistribution
