#ifndef POINCARE_POISSON_DISTRIBUTION_H
#define POINCARE_POISSON_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/discrete_distribution.h>
#include <poincare/preferences.h>

namespace Poincare {

class PoissonDistribution final : public DiscreteDistribution {
public:
  Type type() const override { return Type::Poisson; }
  bool isSymmetrical() const override { return false; }

  template<typename T> static T EvaluateAtAbscissa(T x, const T lambda);
  float EvaluateAtAbscissa(float x, const float * parameters) const override { return EvaluateAtAbscissa<float>(x, parameters[0]); }
  double EvaluateAtAbscissa(double x, const double * parameters) const override { return EvaluateAtAbscissa<double>(x, parameters[0]); }

  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, const T lambda);
  float CumulativeDistributiveInverseForProbability(float x, const float * parameters) const override { return CumulativeDistributiveInverseForProbability<float>(x, parameters[0]); }
  double CumulativeDistributiveInverseForProbability(double x, const double * parameters) const override { return CumulativeDistributiveInverseForProbability<double>(x, parameters[0]); }

  bool ParametersAreOK(const float * parameters) const override { return LambdaIsOK(parameters[0]); }
  bool ParametersAreOK(const double * parameters) const override { return LambdaIsOK(parameters[0]); }

  static bool ExpressionLambdaIsOK(bool * result, const Expression &lambda, Context * context);
  bool ExpressionParametersAreOK(bool * result, const Expression * parameters, Context * context) const override { return ExpressionLambdaIsOK(result, parameters[0], context); }

private:
  template<typename T> static T parameterLambda(T* parameters) { return parameters[0]; }
  template<typename T> static bool LambdaIsOK(T lambda);
};

}

#endif
