#ifndef PROBABILITE_POISSON_LAW_H
#define PROBABILITE_POISSON_LAW_H

#include "one_parameter_law.h"

namespace Probability {

class PoissonLaw : public OneParameterLaw {
public:
  PoissonLaw();
  ~PoissonLaw() override {};
  const char * title() override;
  Type type() const override;
  bool isContinuous() override;
  float xMin() override;
  float yMin() override;
  float xMax() override;
  float yMax() override;
  const char * parameterNameAtIndex(int index) override;
  const char * parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
};

}

#endif
