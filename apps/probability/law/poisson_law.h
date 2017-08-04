#ifndef PROBABILITE_POISSON_LAW_H
#define PROBABILITE_POISSON_LAW_H

#include "one_parameter_law.h"

namespace Probability {

class PoissonLaw : public OneParameterLaw {
public:
  PoissonLaw();
  I18n::Message title() override;
  Type type() const override;
  bool isContinuous() const override;
  float xMin() override;
  float yMin() override;
  float xMax() override;
  float yMax() override;
  I18n::Message parameterNameAtIndex(int index) override;
  I18n::Message parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(double x, int index) const override;
};

}

#endif
