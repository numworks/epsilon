#include <poincare/normal_distribution.h>
#include <poincare/erf_inv.h>
#include <poincare/rational.h>
#include <cmath>
#include <float.h>
#include <assert.h>

namespace Poincare {

template<typename T>
T NormalDistribution::EvaluateAtAbscissa(T x, T mu, T var) {
  if (std::isnan(x) || std::isinf(x) || !ParametersAreOK(mu, var)){
    return NAN;
  }
  const float xMinusMuOverVar = (x - mu)/var;
  return ((T)1.0)/(std::fabs(var) * std::sqrt(((T)2.0) * M_PI)) * std::exp(-((T)0.5) * xMinusMuOverVar * xMinusMuOverVar);
}

template<typename T>
T NormalDistribution::CumulativeDistributiveFunctionAtAbscissa(T x, T mu, T var) {
  if (!ParametersAreOK(mu, var)) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveFunctionAtAbscissa<T>((x-mu)/std::fabs(var));
}

template<typename T>
T NormalDistribution::CumulativeDistributiveInverseForProbability(T probability, T mu, T var) {
  if (!ParametersAreOK(mu, var)) {
    return NAN;
  }
  return StandardNormalCumulativeDistributiveInverseForProbability(probability) * std::fabs(var) + mu;
}

template<typename T>
bool NormalDistribution::ParametersAreOK(T mu, T var) {
  return !std::isnan(mu) && !std::isnan(var)
    && !std::isinf(mu) && !std::isinf(var)
    && var > (T)0.0;
}

bool NormalDistribution::ExpressionParametersAreOK(bool * result, const Expression & mu, const Expression & var, Context * context) {
  assert(result != nullptr);
  if (mu.deepIsMatrix(context) || var.deepIsMatrix(context)) {
    *result = false;
    return true;
  }

  if (!mu.isReal(context) || !var.isReal(context)) {
    // We cannot check that mu and variance are real
    return false;
  }

  {
    ExpressionNode::Sign s = var.sign(context);
    if (s == ExpressionNode::Sign::Negative) {
      *result = false;
      return true;
    }
    // We cannot check that the variance is positive
    if (s != ExpressionNode::Sign::Positive) {
      return false;
    }
  }

  if (var.type() != ExpressionNode::Type::Rational) {
    // We cannot check that the variance is not null
    return false;
  }

  const Rational rationalVar = static_cast<const Rational &>(var);
  if (rationalVar.isZero()) {
    *result = false;
    return true;
  }
  *result = true;
  return true;
}

template<typename T>
T NormalDistribution::StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa) {
  if (std::isnan(abscissa) || std::isinf(abscissa)) {
    return NAN;
  }
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
  if (probability > (T)1.0 || probability < (T)0.0 || std::isnan(probability) || std::isinf(probability)) {
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
template bool NormalDistribution::ParametersAreOK(float mu, float var);
template bool NormalDistribution::ParametersAreOK(double mu, double var);

}
