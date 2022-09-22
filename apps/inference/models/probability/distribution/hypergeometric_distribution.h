#ifndef PROBABILITE_HYPERGEOMETRIC_DISTRIBUTION_H
#define PROBABILITE_HYPERGEOMETRIC_DISTRIBUTION_H

#include "three_parameters_distribution.h"
#include <poincare/layout_helper.h>

namespace Inference {

class HypergeometricDistribution final : public ThreeParametersDistribution {
public:
  HypergeometricDistribution() : ThreeParametersDistribution(Poincare::Distribution::Type::Hypergeometric, 100, 60, 50) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::HypergeometricDistribution; }
  const char * parameterNameAtIndex(int index) const override {
    const char * parametersNames[] = {"N", "K", "n"};
    return parametersNames[index];
  }
  bool authorizedParameterAtIndex(double x, int index) const override;
  double defaultComputedValue() const override { return 1.0; }
private:
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    I18n::Message parameterTexts[] = {I18n::Message::PopulationSize, I18n::Message::TotalItemsWithFeature, I18n::Message::SampleSize};
    return ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(i)), parameterTexts[i]};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
