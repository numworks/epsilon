#ifndef POINCARE_BINOMIAL_DISTRIBUTION_H
#define POINCARE_BINOMIAL_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/discrete_distribution.h>
#include <poincare/preferences.h>

namespace Poincare {

class BinomialDistribution final : public DiscreteDistribution {
public:
  Type type() const override { return Type::Binomial; }
  bool isSymmetrical() const override { return false; }

  template<typename T> static T EvaluateAtAbscissa(T x, const T n, const T p);
  float evaluateAtAbscissa(float x, const float * parameters) const override { return EvaluateAtAbscissa<float>(x, parameters[0], parameters[1]); }
  double evaluateAtAbscissa(double x, const double * parameters) const override { return EvaluateAtAbscissa<double>(x, parameters[0], parameters[1]); }

  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, const T n, const T p);
  float cumulativeDistributiveFunctionAtAbscissa(float x, const float * parameters) const override { return CumulativeDistributiveFunctionAtAbscissa<float>(x, parameters[0], parameters[1]); }
  double cumulativeDistributiveFunctionAtAbscissa(double x, const double * parameters) const override { return CumulativeDistributiveFunctionAtAbscissa<double>(x, parameters[0], parameters[1]); }

  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, const T n, const T p);
  float cumulativeDistributiveInverseForProbability(float x, const float * parameters) const override { return CumulativeDistributiveInverseForProbability<float>(x, parameters[0], parameters[1]); }
  double cumulativeDistributiveInverseForProbability(double x, const double * parameters) const override { return CumulativeDistributiveInverseForProbability<double>(x, parameters[0], parameters[1]); }

  template<typename T> static bool ParametersAreOK(const T n, const T p);
  bool parametersAreOK(const float * parameters) const override { return ParametersAreOK(parameters[0], parameters[1]); }
  bool parametersAreOK(const double * parameters) const override { return ParametersAreOK(parameters[0], parameters[1]); }

  static bool expressionParametersAreOK(bool * result, const Expression &n, const Expression &p, Context * context);
  bool expressionParametersAreOK(bool * result, const Expression * parameters, Context * context) const override { return expressionParametersAreOK(result, parameters[0], parameters[1], context); }


private:
  template<typename T> static T parameterN(T* parameters) { return parameters[0]; }
  template<typename T> static T parameterP(T* parameters) { return parameters[1]; }
};

}

#endif
