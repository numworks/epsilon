#ifndef POINCARE_STUDENT_DISTRIBUTION_H
#define POINCARE_STUDENT_DISTRIBUTION_H

#include <poincare/expression.h>
#include <poincare/distribution.h>
#include <poincare/preferences.h>

namespace Poincare {

class StudentDistribution final : public Distribution {
public:
  template<typename T> static T EvaluateAtAbscissa(T x, const T k);
  float EvaluateAtAbscissa(float x, const float * parameters) override { return EvaluateAtAbscissa<float>(x, parameters[0]); }
  double EvaluateAtAbscissa(double x, const double * parameters) override { return EvaluateAtAbscissa<double>(x, parameters[0]); }

  template<typename T> static T CumulativeDistributiveFunctionAtAbscissa(T x, const T k);
  float CumulativeDistributiveFunctionAtAbscissa(float x, const float * parameters) override { return CumulativeDistributiveFunctionAtAbscissa<float>(x, parameters[0]); }
  double CumulativeDistributiveFunctionAtAbscissa(double x, const double * parameters) override { return CumulativeDistributiveFunctionAtAbscissa<double>(x, parameters[0]); }

  template<typename T> static T CumulativeDistributiveInverseForProbability(T probability, T k);
  float CumulativeDistributiveInverseForProbability(float x, const float * parameters) override { return CumulativeDistributiveInverseForProbability<float>(x, parameters[0]); }
  double CumulativeDistributiveInverseForProbability(double x, const double * parameters) override { return CumulativeDistributiveInverseForProbability<double>(x, parameters[0]); }

  bool ParametersAreOK(const float * parameters) override { return KIsOK(parameters[0]); }
  bool ParametersAreOK(const double * parameters) override { return KIsOK(parameters[0]); }

  static bool ExpressionKIsOK(bool * result, const Expression &k, Context * context);
  bool ExpressionParametersAreOK(bool * result, const Expression * parameters, Context * context) override { return ExpressionKIsOK(result, parameters[0], context); }

  template <typename T> static T lnCoefficient(T k);
private:
  template<typename T> static T parameterK(T* parameters) { return parameters[0]; }
  template<typename T> static bool KIsOK(T k);
};

}

#endif
