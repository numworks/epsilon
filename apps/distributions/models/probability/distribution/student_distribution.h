#ifndef DISTRIBUTION_STUDENT_DISTRIBUTION_H
#define DISTRIBUTION_STUDENT_DISTRIBUTION_H

#include "one_parameter_distribution.h"
#include <float.h>
#include <poincare/code_point_layout.h>
#include <poincare/layout_helper.h>

namespace Distributions {

class StudentDistribution : public OneParameterDistribution {
public:
  StudentDistribution() : OneParameterDistribution(Poincare::Distribution::Type::Student, 1.0) { computeCurveViewRange(); }
  I18n::Message title() const override { return I18n::Message::StudentDistribution; }
  const char * parameterNameAtIndex(int index) const override { return "k"; }
  double meanAbscissa() override { return 0.0; }
  bool authorizedParameterAtIndex(double x, int index) const override;
private:
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::LayoutHelper::String(parameterNameAtIndex(0)), I18n::Message::DegreesOfFreedomDefinition};
  }
  float computeXMin() const override;
  float computeXMax() const override;
  float computeYMax() const override;
};

}

#endif
