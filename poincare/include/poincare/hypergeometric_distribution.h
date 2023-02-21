#ifndef POINCARE_HYPERGEOMETRIC_DISTRIBUTION_H
#define POINCARE_HYPERGEOMETRIC_DISTRIBUTION_H

#include <poincare/discrete_distribution.h>
#include <poincare/expression.h>

namespace Poincare {

class HypergeometricDistribution final : public DiscreteDistribution {
 public:
  Type type() const override { return Type::Hypergeometric; }
  bool isSymmetrical() const override { return false; }

  template <typename T>
  static T EvaluateAtAbscissa(T x, T N, T K, T n);
  float evaluateAtAbscissa(float x, const float *parameters) const override {
    return EvaluateAtAbscissa<float>(x, parameters[0], parameters[1],
                                     parameters[2]);
  }
  double evaluateAtAbscissa(double x, const double *parameters) const override {
    return EvaluateAtAbscissa<double>(x, parameters[0], parameters[1],
                                      parameters[2]);
  }

  template <typename T>
  static T CumulativeDistributiveInverseForProbability(T probability, T N, T K,
                                                       T n);
  float cumulativeDistributiveInverseForProbability(
      float x, const float *parameters) const override {
    return CumulativeDistributiveInverseForProbability<float>(
        x, parameters[0], parameters[1], parameters[2]);
  }
  double cumulativeDistributiveInverseForProbability(
      double x, const double *parameters) const override {
    return CumulativeDistributiveInverseForProbability<double>(
        x, parameters[0], parameters[1], parameters[2]);
  }

  bool parametersAreOK(const float *parameters) const override {
    return NIsOK(parameters[0]) && KIsOK(parameters[1]) && nIsOK(parameters[2]);
  }
  bool parametersAreOK(const double *parameters) const override {
    return NIsOK(parameters[0]) && KIsOK(parameters[1]) && nIsOK(parameters[2]);
  }

  static bool ExpressionNIsOK(bool *result, const Expression &N,
                              Context *context);
  static bool ExpressionKIsOK(bool *result, const Expression &K,
                              Context *context);
  static bool ExpressionnIsOK(bool *result, const Expression &n,
                              Context *context);
  bool expressionParametersAreOK(bool *result, const Expression *parameters,
                                 Context *context) const override {
    return ExpressionNIsOK(result, parameters[0], context) ||
           ExpressionKIsOK(result, parameters[1], context) ||
           ExpressionnIsOK(result, parameters[2], context);
  }

 private:
  template <typename T>
  static bool NIsOK(T p);
  template <typename T>
  static bool KIsOK(T p);
  template <typename T>
  static bool nIsOK(T p);
};

}  // namespace Poincare

#endif
