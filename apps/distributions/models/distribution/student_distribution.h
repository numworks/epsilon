#ifndef DISTRIBUTION_STUDENT_DISTRIBUTION_H
#define DISTRIBUTION_STUDENT_DISTRIBUTION_H

#include <poincare/layout.h>

#include "one_parameter_distribution.h"

namespace Distributions {

class StudentDistribution : public OneParameterDistribution {
 public:
  StudentDistribution()
      : OneParameterDistribution(Poincare::Distribution::Type::Student) {
    computeCurveViewRange();
  }
  I18n::Message title() const override {
    return I18n::Message::StudentDistribution;
  }
  bool authorizedParameterAtIndex(double x, int index) const override;

 private:
  constexpr static double k_defaultK = 1.0;
  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override {
    return Shared::ParameterRepresentation{
        Poincare::Layout::String(parameterNameAtIndex(0)),
        I18n::Message::DegreesOfFreedomDefinition};
  }
  float privateComputeXMin() const override;
  float privateComputeXMax() const override;
  float computeYMax() const override;
};

}  // namespace Distributions

#endif
