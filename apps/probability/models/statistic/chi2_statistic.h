#ifndef APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H

#include "statistic.h"

namespace Probability {

class Chi2Statistic : public CachedStatistic {
public:
  Chi2Statistic() : m_numberOfParameters(k_maxNumberOfParameters) {}
  void computeTest() override;
  const char * testCriticalValueSymbol() override { return "X2"; }
  const char * estimateSymbol() override { return  ""; }
  I18n::Message estimateDescription() override { return I18n::Message::Default; }
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }

  float normedDensityFunction(float x) override;

  void computeInterval() override {}
  const char * intervalCriticalValueSymbol() override { return ""; }

  // The input number of parameters should be provided by the controller
  void setNumberOfParams(int n) { m_numberOfParameters = n;}
  int numberOfStatisticParameters() const override { return m_numberOfParameters; }

protected:
  const ParameterRepr * paramReprAtIndex(int i) const override { return nullptr; }
  float * paramArray() override { return m_input; }

  // Chi2 specific
  virtual float expectedValue(int index);
  virtual float observedValue(int index);

private:
  static float _zAlpha(float degreesOfFreedom, float significanceLevel);
  static float _pVal(float degreesOfFreedom, float z);
  constexpr static int k_maxNumberOfParameters = 20;
  float m_input[k_maxNumberOfParameters];
  float m_degreesOfFreedom;
  int m_numberOfParameters;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H */
