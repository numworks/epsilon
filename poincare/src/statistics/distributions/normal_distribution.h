#ifndef POINCARE_STATISTICS_PROBABILITY_NORMAL_DISTRIBUTION_H
#define POINCARE_STATISTICS_PROBABILITY_NORMAL_DISTRIBUTION_H

#include <poincare/src/memory/tree.h>

#include "continuous_distribution.h"

namespace Poincare {

namespace Internal {

class NormalDistribution final : public ContinuousDistribution {
 public:
  Type type() const override { return Type::Normal; }
  bool isSymmetrical() const override { return true; }

  template <typename T>
  static T EvaluateAtAbscissa(T x, const T mu, const T sigma);
  float evaluateAtAbscissa(float x, const float* parameters) const override {
    return EvaluateAtAbscissa<float>(x, parameters[0], parameters[1]);
  }
  double evaluateAtAbscissa(double x, const double* parameters) const override {
    return EvaluateAtAbscissa<double>(x, parameters[0], parameters[1]);
  }

  template <typename T>
  static T CumulativeDistributiveFunctionAtAbscissa(T x, const T mu,
                                                    const T sigma);
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
                                                       const T mu,
                                                       const T sigma);
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
    return MuAndSigmaAreOK(parameters[0], parameters[1]);
  }
  bool parametersAreOK(const double* parameters) const override {
    return MuAndSigmaAreOK(parameters[0], parameters[1]);
  }

  static bool ExpressionMuAndSigmaAreOK(bool* result, const Tree* mu,
                                        const Tree* sigma);
  bool expressionParametersAreOK(bool* result,
                                 const Tree** parameters) const override {
    return ExpressionMuAndSigmaAreOK(result, parameters[0], parameters[1]);
  }

  double evaluateParameterForProbabilityAndBound(
      int parameterIndex, const double* parameters, double probability,
      double bound, bool isUpperBound) const override;

 private:
  template <typename T>
  static T parameterMu(T* parameters) {
    return parameters[0];
  }
  template <typename T>
  static T parameterSigma(T* parameters) {
    return parameters[1];
  }
  template <typename T>
  static T StandardNormalCumulativeDistributiveFunctionAtAbscissa(T abscissa);
  template <typename T>
  static T StandardNormalCumulativeDistributiveInverseForProbability(
      T probability);
  template <typename T>
  static bool MuAndSigmaAreOK(T mu, T sigma);
};

}  // namespace Internal

using NormalDistribution = Internal::NormalDistribution;

}  // namespace Poincare

#endif
