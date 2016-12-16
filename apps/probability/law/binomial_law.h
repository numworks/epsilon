#ifndef PROBABILITE_BINOMIAL_LAW_H
#define PROBABILITE_BINOMIAL_LAW_H

#include "two_parameter_law.h"

namespace Probability {

class BinomialLaw : public TwoParameterLaw {
public:
  BinomialLaw();
  ~BinomialLaw() override {};
  const char * title() override;
  Type type() const override;
  bool isContinuous() const override;
  float xMin() override;
  float yMin() override;
  float xMax() override;
  float yMax() override;
  const char * parameterNameAtIndex(int index) override;
  const char * parameterDefinitionAtIndex(int index) override;
  float evaluateAtAbscissa(float x) const override;
  bool authorizedValueAtIndex(float x, int index) const override;
  float cumulativeDistributiveInverseForProbability(float * probability) override;
  float rightIntegralInverseForProbability(float * probability) override;
};

}

#endif
