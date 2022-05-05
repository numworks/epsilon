#include <poincare/geometric_distribution.h>
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
T GeometricDistribution::EvaluateAtAbscissa(T x, T p) {
  if (std::isnan(x) || std::isinf(x) || !PIsOK(p)){
    return NAN;
  }
  constexpr T castedOne = static_cast<T>(1.0);
  if (x < castedOne) {
    return static_cast<T>(0.0);
  }
  if (p == castedOne) {
    return x == castedOne ? castedOne : static_cast<T>(0.0);
  }
  // The result is p * (1-p)^{k-1}
  T lResult = (std::floor(x) - castedOne) * std::log(castedOne - p);
  return p * std::exp(lResult);
}

template<typename T>
T GeometricDistribution::CumulativeDistributiveInverseForProbability(T probability, T p) {
  if (!PIsOK(p) || std::isnan(probability) || std::isinf(probability) || probability < static_cast<T>(0.0) || probability > static_cast<T>(1.0)) {
    return NAN;
  }
  constexpr T precision = Float<T>::Epsilon();
  if (std::abs(probability) < precision) {
    return NAN;
  }
  if (std::abs(probability - static_cast<T>(1.0)) < precision) {
    if (std::abs(p - static_cast<T>(1.0)) < precision) {
      return static_cast<T>(1.0);
    }
    return INFINITY;
  }
  T proba = probability;
  const void * pack[1] = { &p };
  /* It works even if G(p) is defined on N* and not N because G(0) returns 0 and
   * not undef */
  return Solver::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](double x, Context * context, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        double p = *static_cast<const T *>(pack[0]);
        return (double)GeometricDistribution::EvaluateAtAbscissa<T>(x, p);
      }, nullptr, pack);
}

template<typename T>
bool GeometricDistribution::PIsOK(T p) {
  return !std::isnan(p) && !std::isinf(p)
    && (p > static_cast<T>(0.0))
    && (p <= static_cast<T>(1.0));
}

bool GeometricDistribution::ExpressionPIsOK(bool * result, const Expression & p, Context * context) {
  assert(result != nullptr);
  if (p.deepIsMatrix(context)) {
    *result = false;
    return true;
  }

  if (p.isUndefined() || Expression::IsInfinity(p, context)) {
    *result = false;
    return true;
  }
  if (!p.isReal(context)) {
    // We cannot check that p is real
    return false;
  }

  if (p.type() != ExpressionNode::Type::Rational) {
    // We cannot check that 0 < p <= 1
    return false;
  }

  {
    const Rational rationalP = static_cast<const Rational &>(p);
    if (rationalP.isNegative() || rationalP.isZero()) {
      // p is negative or null
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
  }

  *result = true;
  return true;
}


template float GeometricDistribution::EvaluateAtAbscissa<float>(float, float);
template double GeometricDistribution::EvaluateAtAbscissa<double>(double, double);
template float GeometricDistribution::CumulativeDistributiveInverseForProbability<float>(float, float);
template double GeometricDistribution::CumulativeDistributiveInverseForProbability<double>(double, double);
template bool GeometricDistribution::PIsOK(float);
template bool GeometricDistribution::PIsOK(double);

}
