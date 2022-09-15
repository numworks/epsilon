#include <poincare/geometric_distribution.h>
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
  return SolverAlgorithms::CumulativeDistributiveInverseForNDefinedFunction<T>(
      &proba,
      [](T x, const void * auxiliary) {
        const void * const * pack = static_cast<const void * const *>(auxiliary);
        T p = *static_cast<const T *>(pack[0]);
        return GeometricDistribution::EvaluateAtAbscissa(x, p);
      }, pack);
}

template<typename T>
bool GeometricDistribution::PIsOK(T p) {
  return Domain::Contains(p, Domain::Type::LeftOpenUnitSegment);
}

bool GeometricDistribution::ExpressionPIsOK(bool * result, const Expression & p, Context * context) {
  return Domain::ExpressionIsIn(result, p, Domain::Type::LeftOpenUnitSegment, context);
}


template float GeometricDistribution::EvaluateAtAbscissa<float>(float, float);
template double GeometricDistribution::EvaluateAtAbscissa<double>(double, double);
template float GeometricDistribution::CumulativeDistributiveInverseForProbability<float>(float, float);
template double GeometricDistribution::CumulativeDistributiveInverseForProbability<double>(double, double);
template bool GeometricDistribution::PIsOK(float);
template bool GeometricDistribution::PIsOK(double);

}
