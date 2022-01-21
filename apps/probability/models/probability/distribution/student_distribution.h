#ifndef DISTRIBUTION_STUDENT_DISTRIBUTION_H
#define DISTRIBUTION_STUDENT_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <float.h>
#include <poincare/code_point_layout.h>

namespace Probability {

class StudentDistribution : public OneParameterDistribution {
public:
  StudentDistribution() : OneParameterDistribution(1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::StudentDistribution; }
  Type type() const override { return Type::Student; }
  bool isContinuous() const override { return true; }
  bool isSymmetrical() const override { return true; }
  double meanAbscissa() override { return 0.0; }
  float evaluateAtAbscissa(float x) const override;
  bool authorizedParameterAtIndex(double x, int index) const override;
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override;
  double cumulativeDistributiveInverseForProbability(double * distribution) override;
private:
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::CodePointLayout::Builder('k'), I18n::Message::DegreesOfFreedomDefinition};
  }
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
