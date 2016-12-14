#ifndef PROBABILITE_UNIFORM_LAW_H
#define PROBABILITE_UNIFORM_LAW_H

#include "two_parameter_law.h"

namespace Probability {

class UniformLaw : public TwoParameterLaw {
public:
  UniformLaw(EvaluateContext * evaluateContext);
  ~UniformLaw() override;
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
