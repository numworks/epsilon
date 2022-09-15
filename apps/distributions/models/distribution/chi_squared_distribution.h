#ifndef DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H
#define DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H

#include <float.h>
#include "one_parameter_distribution.h"
#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>

namespace Distributions {

class ChiSquaredDistribution : public OneParameterDistribution {
public:
  ChiSquaredDistribution() : OneParameterDistribution(Poincare::Distribution::Type::ChiSquared, 1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::ChiSquareDistribution; }
  const char * parameterNameAtIndex(int index) const override { return "k"; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double p) const override;
private:
  constexpr static double k_maxK = 31500.0;
  Shared::ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return Shared::ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(0)), I18n::Message::DegreesOfFreedomDefinition};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
