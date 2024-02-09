#ifndef POINCARE_UNIFORM_DISTRIBUTION_H
#define POINCARE_UNIFORM_DISTRIBUTION_H

#include <poincare/continuous_distribution.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace Poincare {

class UniformDistribution final : public ContinuousDistribution {
 public:
  Type type() const override { return Type::Uniform; }
  bool isSymmetrical() const override { return true; }

  template <typename T>
  static T EvaluateAtAbscissa(T x, const T d1, const T d2);
  float evaluateAtAbscissa(float x, const float* parameters) const override {
    return EvaluateAtAbscissa<float>(x, parameters[0], parameters[1]);
  }
  double evaluateAtAbscissa(double x, const double* parameters) const override {
    return EvaluateAtAbscissa<double>(x, parameters[0], parameters[1]);
  }

  template <typename T>
  static T CumulativeDistributiveFunctionAtAbscissa(T x, const T d1,
                                                    const T d2);
  float cumulativeDistributiveFunctionAtAbscissa(
      float x, const float* parameters) const override {
    return CumulativeDistributiveFunctionAtAbscissa<float>(x, parameters[0],
                                                           parameters[1]);
  }
  double cumulativeDistributiveFunctionAtAbscissa(
      double x, const double* parameters) const override {
    return CumulativeDistributiveFunctionAtAbscissa<double>(x, parameters[0],
                                                            parameters[1]);
  }

  template <typename T>
  static T CumulativeDistributiveInverseForProbability(T probability,
                                                       const T d1, const T d2);
  float cumulativeDistributiveInverseForProbability(
      float x, const float* parameters) const override {
    return CumulativeDistributiveInverseForProbability<float>(x, parameters[0],
                                                              parameters[1]);
  }
  double cumulativeDistributiveInverseForProbability(
      double x, const double* parameters) const override {
    return CumulativeDistributiveInverseForProbability<double>(x, parameters[0],
                                                               parameters[1]);
  }

  bool parametersAreOK(const float* parameters) const override {
    return D1AndD2AreOK(parameters[0], parameters[1]);
  }
  bool parametersAreOK(const double* parameters) const override {
    return D1AndD2AreOK(parameters[0], parameters[1]);
  }

  static bool ExpressionD1AndD2AreOK(bool* result, const Expression& d1,
                                     const Expression& d2, Context* context);
  bool expressionParametersAreOK(bool* result, const Expression* parameters,
                                 Context* context) const override {
    return ExpressionD1AndD2AreOK(result, parameters[0], parameters[1],
                                  context);
  }

 private:
  template <typename T>
  static T parameterD1(T* parameters) {
    return parameters[0];
  }
  template <typename T>
  static T parameterD2(T* parameters) {
    return parameters[1];
  }
  template <typename T>
  static bool D1AndD2AreOK(T d1, T d2);
};

}  // namespace Poincare

#endif
