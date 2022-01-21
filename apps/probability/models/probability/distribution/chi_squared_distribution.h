#ifndef DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H
#define DISTRIBUTION_CHI_SQUARED_DISTRIBUTION_H

#include <float.h>
#include "one_parameter_distribution.h"
#include <poincare/code_point_layout.h>

namespace Probability {

class ChiSquaredDistribution : public OneParameterDistribution {
public:
  ChiSquaredDistribution() : OneParameterDistribution(1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::ChiSquaredDistribution; }
  Type type() const override { return Type::ChiSquared; }
  bool isContinuous() const override { return true; }
  bool isSymmetrical() const override { return false; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * p) override;
private:
  static constexpr double k_maxK = 31500.0;
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::CodePointLayout::Builder('k'), I18n::Message::DegreesOfFreedomDefinition};
  }
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
