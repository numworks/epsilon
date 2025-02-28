#include <poincare/src/statistics/distributions/discrete_distribution.h>

#include "poincare/statistics/distribution.h"

namespace Poincare::Internal::DiscreteDistribution {

template <typename T>
T CumulativeDistributiveFunctionAtAbscissa(Distribution::Type distribType, T x,
                                           const T* parameters) {
  if (std::isinf(x)) {
    return x > static_cast<T>(0.0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (x < static_cast<T>(0.0)) {
    return static_cast<T>(0.0);
  }
  const void* pack[2] = {&distribType, parameters};
  return SolverAlgorithms::CumulativeDistributiveFunctionForNDefinedFunction<T>(
      x,
      [](T k, const void* auxiliary) {
        const void* const* pack = static_cast<const void* const*>(auxiliary);
        Distribution::Type* type = const_cast<Distribution::Type*>(
            static_cast<const Distribution::Type*>(pack[0]));
        const T* parameters = static_cast<const T*>(pack[1]);
        return Distribution(*type).evaluateAtAbscissa(k, parameters);
      },
      pack);
}

template float CumulativeDistributiveFunctionAtAbscissa<float>(
    Distribution::Type, float, const float*);
template double CumulativeDistributiveFunctionAtAbscissa<double>(
    Distribution::Type, double, const double*);

}  // namespace Poincare::Internal::DiscreteDistribution
