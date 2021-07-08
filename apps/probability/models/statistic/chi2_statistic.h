#ifndef APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H

#include "statistic.h"

namespace Probability {

class Chi2Statistic : public CachedStatistic {
public:
  void computeTest() override;
  const char * testCriticalValueSymbol() override { return "X2"; }
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }

  float normedDensityFunction(float x) override;

  void computeInterval() override {}
  const char * intervalCriticalValueSymbol() override { return ""; }

protected:
  const ParameterRepr * paramReprAtIndex(int i) const override { return nullptr; }
  float * paramArray() override { return m_input; }

  // Chi2 specific
  virtual float expectedValue(int index);
  virtual float observedValue(int index);

private:
  float _zAlpha(float degreesOfFreedom, float significanceLevel);
  float _pVal(float degreesOfFreedom, float z);
  constexpr static int k_maxNumberOfParameters = 10;
  float m_input[k_maxNumberOfParameters];
  float m_degreesOfFreedom;
};

} // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H */
