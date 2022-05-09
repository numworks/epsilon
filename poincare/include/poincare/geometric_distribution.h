#ifndef POINCARE_GEOMETRIC_DISTRIBUTION_H
#define POINCARE_GEOMETRIC_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/discrete_distribution.h>
#include <poincare/preferences.h>

namespace Poincare {

class GeometricDistribution final : public DiscreteDistribution {
public:
  template<typename T> static T EvaluateAtAbscissa(T x, const T p);
  float EvaluateAtAbscissa(float x, const float * parameters) const override { return EvaluateAtAbscissa<float>(x, parameters[0]); }
  double EvaluateAtAbscissa(double x, const double * parameters) const override { return EvaluateAtAbscissa<double>(x, parameters[0]); }

  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, T lambda);
  float CumulativeDistributiveInverseForProbability(float x, const float * parameters) const override { return CumulativeDistributiveInverseForProbability<float>(x, parameters[0]); }
  double CumulativeDistributiveInverseForProbability(double x, const double * parameters) const override { return CumulativeDistributiveInverseForProbability<double>(x, parameters[0]); }

  bool ParametersAreOK(const float * parameters) const override { return PIsOK(parameters[0]); }
  bool ParametersAreOK(const double * parameters) const override { return PIsOK(parameters[0]); }

  static bool ExpressionPIsOK(bool * result, const Expression &p, Context * context);
  bool ExpressionParametersAreOK(bool * result, const Expression * parameters, Context * context) const override { return ExpressionPIsOK(result, parameters[0], context); }

private:
  template<typename T> static T parameterP(T* parameters) { return parameters[0]; }
  template<typename T> static bool PIsOK(T p);
};

}

#endif
