#include <poincare/exponential_distribution.h>
#include <poincare/domain.h>
#include <poincare/regularized_incomplete_beta_function.h>
#include <cmath>
#include <float.h>

namespace Poincare {

template <typename T> T ExponentialDistribution::EvaluateAtAbscissa(T x, T lambda) {
  if (x < static_cast<T>(0.0)) {
    return NAN;
  }
  return lambda * std::exp(-lambda * x);
}

template <typename T> T ExponentialDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T lambda) {
  if (x < 0.0) {
    return static_cast<T>(0.0);
  }
  return static_cast<T>(1.0) - std::exp((-lambda * x));
}

template <typename T> T ExponentialDistribution::CumulativeDistributiveInverseForProbability(T probability, T lambda) {
  if (probability >= static_cast<T>(1.0)) {
    return INFINITY;
  }
  if (probability <= static_cast<T>(0.0)) {
    return static_cast<T>(0.0);
  }
  return -std::log(1.0 - probability)/lambda;
}

template<typename T>
bool ExponentialDistribution::LambdaIsOK(T lambda) {
  return !std::isnan(lambda) && !std::isinf(lambda)
   && lambda >= static_cast<T>(0.0) && lambda <= static_cast<T>(7500.0);
}

bool ExponentialDistribution::ExpressionLambdaIsOK(bool * result, const Expression & lambda, Context * context) {
  return Domain::expressionIsIn(result, lambda, Domain::Type::RPlusStar, context);
}

// Specialisations
template float ExponentialDistribution::EvaluateAtAbscissa<float>(float, float);
template double ExponentialDistribution::EvaluateAtAbscissa<double>(double, double);
template float ExponentialDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float);
template double ExponentialDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double);
template float ExponentialDistribution::CumulativeDistributiveInverseForProbability<float>(float, float);
template double ExponentialDistribution::CumulativeDistributiveInverseForProbability<double>(double, double);
template bool ExponentialDistribution::LambdaIsOK(float);
template bool ExponentialDistribution::LambdaIsOK(double);

}
