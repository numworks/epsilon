#include "hypergeometric_distribution.h"

#include <assert.h>
#include <float.h>
#include <omg/float.h>
#include <poincare/old/binomial_coefficient.h>
#include <poincare/src/numeric/solver_algorithms.h>

#include <cmath>

#include "domain.h"

namespace Poincare::Internal {

template <typename T>
T HypergeometricDistribution::EvaluateAtAbscissa(T k, T N, T K, T n) {
  if (!std::isfinite(k) || n > N || K > N ||
      (N - K == N && K != static_cast<T>(0.))) {
    /* `N - K == N` checks if K is negligeable compared to N, to avoid precision
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
  T result = BinomialCoefficientNode::compute(k, K) *
             BinomialCoefficientNode::compute(n - k, N - K) /
             BinomialCoefficientNode::compute(n, N);
  if (result < 0 || result > 1) {
    // Precision errors
    return NAN;
  }
  return result;
}

template <typename T>
T HypergeometricDistribution::CumulativeDistributiveInverseForProbability(
    T probability, T N, T K, T n) {
  if (!std::isfinite(probability) || probability < static_cast<T>(0.0) ||
      probability > static_cast<T>(1.0)) {
    return NAN;
  }
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
  const void* pack[3] = {&N, &K, &n};
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void* auxiliary) {
        const void* const* pack = static_cast<const void* const*>(auxiliary);
        T N = *static_cast<const T*>(pack[0]);
        T K = *static_cast<const T*>(pack[1]);
        T n = *static_cast<const T*>(pack[2]);
        return HypergeometricDistribution::EvaluateAtAbscissa(x, N, K, n);
      },
      pack);
}

template <typename T>
bool HypergeometricDistribution::NIsOK(T N) {
  return Domain::Contains(N, Domain::Type::N);
}

template <typename T>
bool HypergeometricDistribution::KIsOK(T K) {
  return Domain::Contains(K, Domain::Type::N);  // && K <= N
}

template <typename T>
bool HypergeometricDistribution::nIsOK(T n) {
  return Domain::Contains(n, Domain::Type::N);  // && n <= N
}

bool HypergeometricDistribution::ExpressionNIsOK(bool* result, const Tree* N,
                                                 Context* context) {
  return Domain::ExpressionIsIn(result, N, Domain::Type::N, context);
}

bool HypergeometricDistribution::ExpressionKIsOK(bool* result, const Tree* K,
                                                 Context* context) {
  return Domain::ExpressionIsIn(result, K, Domain::Type::N, context);
}

bool HypergeometricDistribution::ExpressionnIsOK(bool* result, const Tree* n,
                                                 Context* context) {
  return Domain::ExpressionIsIn(result, n, Domain::Type::N, context);
}

template float HypergeometricDistribution::EvaluateAtAbscissa<float>(float,
                                                                     float,
                                                                     float,
                                                                     float);
template double HypergeometricDistribution::EvaluateAtAbscissa<double>(double,
                                                                       double,
                                                                       double,
                                                                       double);
template float
HypergeometricDistribution::CumulativeDistributiveInverseForProbability<float>(
    float, float, float, float);
template double
HypergeometricDistribution::CumulativeDistributiveInverseForProbability<double>(
    double, double, double, double);
template bool HypergeometricDistribution::NIsOK(float);
template bool HypergeometricDistribution::NIsOK(double);
template bool HypergeometricDistribution::KIsOK(float);
template bool HypergeometricDistribution::KIsOK(double);
template bool HypergeometricDistribution::nIsOK(float);
template bool HypergeometricDistribution::nIsOK(double);

}  // namespace Poincare::Internal
