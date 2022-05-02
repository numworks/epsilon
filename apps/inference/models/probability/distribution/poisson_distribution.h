#ifndef PROBABILITE_POISSON_DISTRIBUTION_H
#define PROBABILITE_POISSON_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <poincare/layout_helper.h>

namespace Inference {

class PoissonDistribution final : public OneParameterDistribution {
public:
  PoissonDistribution() : OneParameterDistribution(4.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::PoissonDistribution; }
  Type type() const override { return Type::Poisson; }
  const char * parameterNameAtIndex(int index) const override { return "λ"; }
  bool isContinuous() const override { return false; }
  bool isSymmetrical() const override { return false; }
  float evaluateAtAbscissa(float x) const override {
    return templatedApproximateAtAbscissa<float>(x);
  }
  bool authorizedParameterAtIndex(double x, int index) const override;
private:
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(0)), I18n::Message::LambdaPoissonDefinition};
  }
  float computeXMax() const override;
  float computeYMax() const override;
  double evaluateAtDiscreteAbscissa(int k) const override {
    return templatedApproximateAtAbscissa<double>(static_cast<double>(k));
  }
  template<typename T> T templatedApproximateAtAbscissa(T x) const;
};

}

#endif
