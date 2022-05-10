#ifndef PROBABILITE_POISSON_DISTRIBUTION_H
#define PROBABILITE_POISSON_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <poincare/layout_helper.h>

namespace Inference {

class PoissonDistribution final : public OneParameterDistribution {
public:
  PoissonDistribution() : OneParameterDistribution(Poincare::Distribution::Type::Poisson, 4.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::PoissonDistribution; }
  Type type() const override { return Type::Poisson; }
  const char * parameterNameAtIndex(int index) const override { return "λ"; }
  bool authorizedParameterAtIndex(double x, int index) const override;
private:
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(0)), I18n::Message::LambdaPoissonDefinition};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
