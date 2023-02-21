#ifndef POINCARE_POISSON_DISTRIBUTION_H
#define POINCARE_POISSON_DISTRIBUTION_H

#include <poincare/discrete_distribution.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace Poincare {

class PoissonDistribution final : public DiscreteDistribution {
 public:
  Type type() const override { return Type::Poisson; }
  bool isSymmetrical() const override { return false; }

  template <typename T>
  static T EvaluateAtAbscissa(T x, const T lambda);
  float evaluateAtAbscissa(float x, const float* parameters) const override {
    return EvaluateAtAbscissa<float>(x, parameters[0]);
  }
  double evaluateAtAbscissa(double x, const double* parameters) const override {
    return EvaluateAtAbscissa<double>(x, parameters[0]);
  }

  template <typename T>
  static T CumulativeDistributiveInverseForProbability(T probability,
                                                       const T lambda);
  float cumulativeDistributiveInverseForProbability(
      float x, const float* parameters) const override {
    return CumulativeDistributiveInverseForProbability<float>(x, parameters[0]);
  }
  double cumulativeDistributiveInverseForProbability(
      double x, const double* parameters) const override {
    return CumulativeDistributiveInverseForProbability<double>(x,
                                                               parameters[0]);
  }

  bool parametersAreOK(const float* parameters) const override {
    return LambdaIsOK(parameters[0]);
  }
  bool parametersAreOK(const double* parameters) const override {
    return LambdaIsOK(parameters[0]);
  }

  static bool ExpressionLambdaIsOK(bool* result, const Expression& lambda,
                                   Context* context);
  bool expressionParametersAreOK(bool* result, const Expression* parameters,
                                 Context* context) const override {
    return ExpressionLambdaIsOK(result, parameters[0], context);
  }

 private:
  template <typename T>
  static T parameterLambda(T* parameters) {
    return parameters[0];
  }
  template <typename T>
  static bool LambdaIsOK(T lambda);
};

}  // namespace Poincare

#endif
