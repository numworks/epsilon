#include <poincare/discrete_distribution.h>

namespace Poincare {

template<typename T> T DiscreteDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, const T * parameters) const {
  if (!parametersAreOK(parameters) || std::isnan(x)) {
    return NAN;
  }
  if (std::isinf(x)) {
    return x > static_cast<T>(0.0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (x < static_cast<T>(0.0)) {
    return static_cast<T>(0.0);
  }
  const void * pack[2] = { this, parameters };
  return Solver::CumulativeDistributiveFunctionForNDefinedFunction<T>(x,
        [](T k, Poincare::Context * context, const void * auxiliary) {
          const void * const * pack = static_cast<const void * const *>(auxiliary);
          Distribution * distribution = const_cast<Distribution *>(static_cast<const Distribution *>(pack[0]));
          const T * parameters = static_cast<const T *>(pack[1]);
          return distribution->evaluateAtAbscissa(k, parameters);
        },
        nullptr, pack);
}

template float DiscreteDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, const float *) const;
template double DiscreteDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, const double *) const;

}
