#ifndef PROBABILITE_EXPONENTIAL_DISTRIBUTION_H
#define PROBABILITE_EXPONENTIAL_DISTRIBUTION_H

#include <assert.h>
#include "one_parameter_distribution.h"
#include <poincare/layout_helper.h>

namespace Probability {

class ExponentialDistribution final : public OneParameterDistribution {
public:
  ExponentialDistribution() : OneParameterDistribution(1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::ExponentialDistribution; }
  Type type() const override { return Type::Exponential; }
  const char * parameterNameAtIndex(int index) const override { return "λ"; }
  bool isContinuous() const override { return true; }
  bool isSymmetrical() const override { return false; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * p) override;
private:
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(0)), I18n::Message::LambdaExponentialDefinition};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
