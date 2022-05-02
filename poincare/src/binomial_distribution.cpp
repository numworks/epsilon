#include <poincare/binomial_distribution.h>
#include <poincare/float.h>
#include <poincare/rational.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Poincare {

template<typename T>
T BinomialDistribution::EvaluateAtAbscissa(T x, T n, T p) {
  if (std::isnan(x) || std::isinf(x) || !ParametersAreOK(n, p)){
    return NAN;
  }
  constexpr T precision = Float<T>::Epsilon();
  bool nIsZero = std::abs(n) < precision;
  bool pIsZero = std::abs(p) < precision;
  bool pIsOne = !pIsZero && std::abs(p - static_cast<T>(1.0)) < precision;
  if (nIsZero) {
    if (pIsZero || pIsOne) {
      return NAN;
    }
    if (std::floor(x) == 0) {
      return static_cast<T>(1.0);
    }
    return static_cast<T>(0.0);
  }
  if (pIsOne) {
    return static_cast<T>(floor(x) == n ? 1.0 : 0.0);
  }
  if (pIsZero) {
    return static_cast<T>(floor(x) == 0 ? 1.0 : 0.0);
  }
  if (x > n) {
    return static_cast<T>(0.0);
  }
  T lResult = std::lgamma(n+static_cast<T>(1.0)) - std::lgamma(std::floor(x)+static_cast<T>(1.0)) - std::lgamma(n - std::floor(x)+static_cast<T>(1.0)) +
    std::floor(x)*std::log(p) + (n-std::floor(x))*std::log(static_cast<T>(1.0)-p);
  return std::exp(lResult);
}

template<typename T>
T BinomialDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T n, T p) {
  if (!ParametersAreOK(n, p) || std::isnan(x)) {
    return NAN;
  }
  if (std::isinf(x)) {
    return x > static_cast<T>(0.0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (x < static_cast<T>(0.0)) {
    return static_cast<T>(0.0);
  }
  if (x >= n) {
    return static_cast<T>(1.0);
  }
  T floorX = std::floor(x);
  return RegularizedIncompleteBetaFunction(n-floorX, floorX+1.0, 1.0-p);
}

template<typename T>
T BinomialDistribution::CumulativeDistributiveInverseForProbability(T probability, T n, T p) {
  if (!ParametersAreOK(n, p) || std::isnan(probability) || std::isinf(probability) || probability < static_cast<T>(0.0) || probability > static_cast<T>(1.0)) {
    return NAN;
  }
  constexpr T precision = Float<T>::Epsilon();
  bool nIsZero = std::abs(n) < precision;
  bool pIsZero = std::abs(p) < precision;
  bool pIsOne = !pIsZero && std::abs(p - static_cast<T>(1.0)) < precision;
  if (nIsZero && (pIsZero || pIsOne)) {
    return NAN;
  }
  if (std::abs(probability) < precision) {
    if (pIsOne) {
      return 0;
    }
    return NAN;
  }
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    return n;
  }
  T proba = probability;
  const void * pack[2] = { &n, &p };
  return Solver::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](double x, Context * context, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        double n = *static_cast<const T *>(pack[0]);
        double p = *static_cast<const T *>(pack[1]);
        return (double)BinomialDistribution::EvaluateAtAbscissa<T>(x, n, p);
      }, nullptr, pack);
}

template<typename T>
bool BinomialDistribution::ParametersAreOK(T n, T p) {
  return !std::isnan(n) && !std::isnan(p)
    && !std::isinf(n) && !std::isinf(p)
    && (n == static_cast<int>(n)) && (n >= static_cast<T>(0.0))
    && (p >= static_cast<T>(0.0)) && (p <= static_cast<T>(1.0));
}

bool BinomialDistribution::ExpressionParametersAreOK(bool * result, const Expression & n, const Expression & p, Context * context) {
  assert(result != nullptr);
  if (n.deepIsMatrix(context) || p.deepIsMatrix(context)) {
    *result = false;
    return true;
  }

  if (n.isUndefined() || p.isUndefined() || Expression::IsInfinity(n, context) || Expression::IsInfinity(p,context)) {
    *result = false;
    return true;
  }
  if (!n.isReal(context) || !p.isReal(context)) {
    // We cannot check that n and p are real
    return false;
  }

  if (n.type() != ExpressionNode::Type::Rational) {
    // We cannot check that that n is an integer
    return false;
  }

  {
    const Rational rationalN = static_cast<const Rational &>(n);
    if (rationalN.isNegative() || !rationalN.isInteger()) {
      // n is negative or not an integer
      *result = false;
      return true;
    }
  }

  if (p.type() != ExpressionNode::Type::Rational) {
    // We cannot check that that p is >= 0 and <= 1
    return false;
  }

  const Rational rationalP = static_cast<const Rational &>(p);
  if (rationalP.isNegative()) {
    // p is negative
    *result = false;
    return true;
  }
  Integer num = rationalP.unsignedIntegerNumerator();
  Integer den = rationalP.integerDenominator();
  if (den.isLowerThan(num)) {
    // p > 1
    *result = false;
    return true;
  }

  *result = true;
  return true;
}


template float BinomialDistribution::EvaluateAtAbscissa<float>(float, float, float);
template double BinomialDistribution::EvaluateAtAbscissa<double>(double, double, double);
template float BinomialDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float, float);
template double BinomialDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double, double);
template float BinomialDistribution::CumulativeDistributiveInverseForProbability<float>(float, float, float);
template double BinomialDistribution::CumulativeDistributiveInverseForProbability<double>(double, double, double);
template bool BinomialDistribution::ParametersAreOK(float, float);
template bool BinomialDistribution::ParametersAreOK(double, double);

}
