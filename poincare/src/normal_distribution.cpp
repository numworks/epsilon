#include <poincare/normal_distribution.h>
#include <poincare/erf_inv.h>
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Poincare {

template<typename T>
T NormalDistribution::EvaluateAtAbscissa(T x, T mu, T sigma) {
  assert(!std::isnan(x) && !std::isnan(mu) && !std::isnan(sigma));
  if (sigma == (T)0.0) {
    return NAN;
  }
  const float xMinusMuOverSigma = (x - mu)/sigma;
  return ((T)1.0)/(std::fabs(sigma) * std::sqrt(((T)2.0) * M_PI)) * std::exp(-((T)0.5) * xMinusMuOverSigma * xMinusMuOverSigma);
}

template<typename T>
T NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T mu, T sigma) {
  if (sigma == (T)0.0) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveFunctionAtAbscissa<T>((x-mu)/std::fabs(sigma));
}

template<typename T>
T NormalDistribution::CumulativeDistributiveInverseForProbability(T probability, T mu, T sigma) {
  if (sigma == (T)0.0) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveInverseForProbability(probability) * std::fabs(sigma) + mu;
}

template<typename T>
T NormalDistribution::StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa) {
  if (abscissa == (T)0.0) {
    return (T)0.5;
  }
  if (abscissa < (T)0.0) {
    return ((T)1.0) - StandardNormalCumulativeDistributiveFunctionAtAbscissa(-abscissa);
  }
  if (abscissa > k_boundStandardNormalDistribution) {
    return (T)1.0;
  }
  return ((T)0.5) + ((T)0.5) * std::erf(abscissa/std::sqrt(((T)2.0)));
}

template<typename T>
T NormalDistribution::StandardNormalCumulativeDistributiveInverseForProbability(T probability) {
  if (probability > (T)1.0 || probability < (T)0.0) {
    return NAN;
  }
  T precision = sizeof(T) == sizeof(double) ? DBL_EPSILON : FLT_EPSILON;
  if (((T)1.0) - probability < precision) {
    return INFINITY;
  }
  if (probability < precision) {
    return -INFINITY;
  }
  if (probability < (T)0.5) {
    return -StandardNormalCumulativeDistributiveInverseForProbability(((T)1.0)-probability);
  }
  return std::sqrt((T)2.0) * erfInv(((T)2.0) * probability - (T)1.0);
}

template float NormalDistribution::EvaluateAtAbscissa<float>(float, float, float);
template double NormalDistribution::EvaluateAtAbscissa<double>(double, double, double);
template float NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float, float);
template double NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double, double);
template float NormalDistribution::CumulativeDistributiveInverseForProbability<float>(float, float, float);
template double NormalDistribution::CumulativeDistributiveInverseForProbability<double>(double, double, double);

}
