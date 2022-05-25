#include <poincare/normal_distribution.h>
#include <poincare/erf_inv.h>
#include <poincare/float.h>
#include <poincare/domain.h>
#include <cmath>
#include <float.h>
#include <assert.h>

#define M_SQRT_2PI 2.506628274631000502415765284811

namespace Poincare {

template<typename T>
T NormalDistribution::EvaluateAtAbscissa(T x, T mu, T sigma) {
  if (std::isnan(x) || std::isinf(x) || !MuAndSigmaAreOK(mu, sigma)){
    return NAN;
  }
  const float xMinusMuOverVar = (x - mu)/sigma;
  return (static_cast<T>(1.0))/(std::fabs(sigma) * static_cast<T>(M_SQRT_2PI)) * std::exp(-(static_cast<T>(0.5)) * xMinusMuOverVar * xMinusMuOverVar);
}

template<typename T>
T NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T mu, T sigma) {
  if (!MuAndSigmaAreOK(mu, sigma)) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveFunctionAtAbscissa<T>((x-mu)/std::fabs(sigma));
}

template<typename T>
T NormalDistribution::CumulativeDistributiveInverseForProbability(T probability, T mu, T sigma) {
  if (!MuAndSigmaAreOK(mu, sigma)) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveInverseForProbability(probability) * std::fabs(sigma) + mu;
}

template<typename T>
bool NormalDistribution::MuAndSigmaAreOK(T mu, T sigma) {
  return !std::isnan(mu) && !std::isnan(sigma)
    && !std::isinf(mu) && !std::isinf(sigma)
    && sigma > static_cast<T>(0.0);
}

bool NormalDistribution::ExpressionMuAndVarAreOK(bool * result, const Expression & mu, const Expression & var, Context * context) {
  return Domain::expressionsAreIn(result, mu, Domain::Type::R, var, Domain::Type::RPlusStar, context);
}

template<typename T>
T NormalDistribution::StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa) {
  if (std::isnan(abscissa)) {
    return NAN;
  }
  if (std::isinf(abscissa)) {
    return abscissa > static_cast<T>(0.0) ? static_cast<T>(1.0) : static_cast<T>(0.0);
  }
  if (abscissa == static_cast<T>(0.0)) {
    return static_cast<T>(0.5);
  }
  if (abscissa < static_cast<T>(0.0)) {
    return (static_cast<T>(1.0)) - StandardNormalCumulativeDistributiveFunctionAtAbscissa(-abscissa);
  }
  return (static_cast<T>(0.5)) + (static_cast<T>(0.5)) * std::erf(abscissa/static_cast<T>(M_SQRT2));
}

template<typename T>
T NormalDistribution::StandardNormalCumulativeDistributiveInverseForProbability(T probability) {
  if (probability > static_cast<T>(1.0) || probability < static_cast<T>(0.0) || std::isnan(probability) || std::isinf(probability)) {
    return NAN;
  }
  constexpr T precision = Float<T>::Epsilon();
  if ((static_cast<T>(1.0)) - probability < precision) {
    return INFINITY;
  }
  if (probability < precision) {
    return -INFINITY;
  }
  if (probability < static_cast<T>(0.5)) {
    return -StandardNormalCumulativeDistributiveInverseForProbability((static_cast<T>(1.0))-probability);
  }
  return static_cast<T>(M_SQRT2) * erfInv((static_cast<T>(2.0)) * probability - static_cast<T>(1.0));
}

template float NormalDistribution::EvaluateAtAbscissa<float>(float, float, float);
template double NormalDistribution::EvaluateAtAbscissa<double>(double, double, double);
template float NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<float>(float, float, float);
template double NormalDistribution::CumulativeDistributiveFunctionAtAbscissa<double>(double, double, double);
template float NormalDistribution::CumulativeDistributiveInverseForProbability<float>(float, float, float);
template double NormalDistribution::CumulativeDistributiveInverseForProbability<double>(double, double, double);
template bool NormalDistribution::MuAndSigmaAreOK(float mu, float sigma);
template bool NormalDistribution::MuAndSigmaAreOK(double mu, double sigma);

}
