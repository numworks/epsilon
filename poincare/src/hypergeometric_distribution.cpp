#include <poincare/hypergeometric_distribution.h>
#include <poincare/binomial_coefficient.h>
#include <poincare/float.h>
#include <poincare/domain.h>
#include <poincare/solver.h>
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Poincare {

template<typename T>
T HypergeometricDistribution::EvaluateAtAbscissa(T k, T N, T K, T n) {
  if (!std::isfinite(k) || n > N || K > N){
    return NAN;
  }
  k = std::floor(k);
  if (k < std::max(static_cast<T>(0), n + K - N) || k > std::min(n, K)) {
    return 0;
  }
  // We don't want BinomialCoefficient to generalize the formula
  if (k > K || (n - k) > (N - K)) {
    return 0;
  }
  return BinomialCoefficientNode::compute(k, K) * BinomialCoefficientNode::compute(n - k, N - K) / BinomialCoefficientNode::compute(n, N);
}

template<typename T>
T HypergeometricDistribution::CumulativeDistributiveInverseForProbability(T probability, T N, T K, T n) {
  if (!std::isfinite(probability) || probability < static_cast<T>(0.0) || probability > static_cast<T>(1.0)) {
    return NAN;
  }
  constexpr T precision = Float<T>::Epsilon();
  if (std::abs(probability) < precision) {
    return NAN;
  }
  T proba = probability;
  const void * pack[3] = { &N, &K, &n };
  return Solver::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, Context * context, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        T N = *static_cast<const T *>(pack[0]);
        T K = *static_cast<const T *>(pack[1]);
        T n = *static_cast<const T *>(pack[2]);
        return HypergeometricDistribution::EvaluateAtAbscissa(x, N, K, n);
      }, nullptr, pack);
}

template<typename T>
bool HypergeometricDistribution::NIsOK(T N) {
  return Domain::Contains(N, Domain::Type::N);
}

template<typename T>
bool HypergeometricDistribution::KIsOK(T K) {
  return Domain::Contains(K, Domain::Type::N); // && K <= N
}

template<typename T>
bool HypergeometricDistribution::nIsOK(T n) {
  return Domain::Contains(n, Domain::Type::N); // && n <= N
}

bool HypergeometricDistribution::ExpressionNIsOK(bool * result, const Expression & N, Context * context) {
  return Domain::ExpressionIsIn(result, N, Domain::Type::N, context);
}

bool HypergeometricDistribution::ExpressionKIsOK(bool * result, const Expression & K, Context * context) {
  return Domain::ExpressionIsIn(result, K, Domain::Type::N, context);
}

bool HypergeometricDistribution::ExpressionnIsOK(bool * result, const Expression & n, Context * context) {
  return Domain::ExpressionIsIn(result, n, Domain::Type::N, context);
}

template float HypergeometricDistribution::EvaluateAtAbscissa<float>(float, float, float, float);
template double HypergeometricDistribution::EvaluateAtAbscissa<double>(double, double, double, double);
template float HypergeometricDistribution::CumulativeDistributiveInverseForProbability<float>(float, float, float, float);
template double HypergeometricDistribution::CumulativeDistributiveInverseForProbability<double>(double, double, double, double);
template bool HypergeometricDistribution::NIsOK(float);
template bool HypergeometricDistribution::NIsOK(double);
template bool HypergeometricDistribution::KIsOK(float);
template bool HypergeometricDistribution::KIsOK(double);
template bool HypergeometricDistribution::nIsOK(float);
template bool HypergeometricDistribution::nIsOK(double);

}
