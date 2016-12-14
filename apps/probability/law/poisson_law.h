#ifndef PROBABILITE_POISSON_LAW_H
#define PROBABILITE_POISSON_LAW_H

#include "one_parameter_law.h"

namespace Probability {

class PoissonLaw : public OneParameterLaw {
public:
  PoissonLaw(EvaluateContext * evaluateContext);
  ~PoissonLaw() override;
  const char * title() override;
  Expression * expression() const override;
  Type type() const override;
  bool isContinuous() override;
  float xMin() override;
  float yMin() override;
  float xMax() override;
  float yMax() override;
  const char * parameterNameAtIndex(int index) override;
  const char * parameterDefinitionAtIndex(int index) override;
private:
  Expression * m_expression;
};

}

#endif
