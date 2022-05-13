#ifndef POINCARE_NORMAL_DISTRIBUTION_H
#define POINCARE_NORMAL_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/continuous_distribution.h>
#include <poincare/preferences.h>

namespace Poincare {

class NormalDistribution final : public ContinuousDistribution {
public:
  Type type() const override { return Type::Normal; }
  bool isSymmetrical() const override { return true; }

  template<typename T> static T EvaluateAtAbscissa(T x, const T mu, const T sigma);
  float EvaluateAtAbscissa(float x, const float * parameters) const override { return EvaluateAtAbscissa<float>(x, parameters[0], parameters[1]); }
  double EvaluateAtAbscissa(double x, const double * parameters) const override { return EvaluateAtAbscissa<double>(x, parameters[0], parameters[1]); }

  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, const T mu, const T sigma);
  float CumulativeDistributiveFunctionAtAbscissa(float x, const float * parameters) const override { return CumulativeDistributiveFunctionAtAbscissa<float>(x, parameters[0], parameters[1]); }
  double CumulativeDistributiveFunctionAtAbscissa(double x, const double * parameters) const override { return CumulativeDistributiveFunctionAtAbscissa<double>(x, parameters[0], parameters[1]); }

  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, const T mu, const T sigma);
  float CumulativeDistributiveInverseForProbability(float x, const float * parameters) const override { return CumulativeDistributiveInverseForProbability<float>(x, parameters[0], parameters[1]); }
  double CumulativeDistributiveInverseForProbability(double x, const double * parameters) const override { return CumulativeDistributiveInverseForProbability<double>(x, parameters[0], parameters[1]); }

  bool ParametersAreOK(const float * parameters) const override { return MuAndSigmaAreOK(parameters[0], parameters[1]); }
  bool ParametersAreOK(const double * parameters) const override { return MuAndSigmaAreOK(parameters[0], parameters[1]); }

  static bool ExpressionMuAndVarAreOK(bool * result, const Expression &mu, const Expression &sigma, Context * context);
  bool ExpressionParametersAreOK(bool * result, const Expression * parameters, Context * context) const override { return ExpressionMuAndVarAreOK(result, parameters[0], parameters[1], context); }

private:
  template<typename T> static T parameterMu(T* parameters) { return parameters[0]; }
  template<typename T> static T parameterSigma(T* parameters) { return parameters[1]; }
  template<typename T> static T StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa);
  template<typename T> static T StandardNormalCumulativeDistributiveInverseForProbability(T probability);
  template<typename T> static bool MuAndSigmaAreOK(T mu, T sigma);
};

}

#endif
