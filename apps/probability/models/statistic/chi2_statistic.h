#ifndef APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H

#include "statistic.h"

#include <cmath>

namespace Probability {

class Chi2Statistic : public CachedStatistic {
public:
  Chi2Statistic();
  void computeTest() override;
  const char * testCriticalValueSymbol() override { return "X2"; }
  const char * estimateSymbol() override { return  ""; }
  I18n::Message estimateDescription() override { return I18n::Message::Default; }
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }

  float normedDensityFunction(float x) override;

  void computeInterval() override {}
  const char * intervalCriticalValueSymbol() override { return ""; }

  int numberOfStatisticParameters() const override { return k_maxNumberOfParameters; };

  constexpr static float k_undefinedValue = NAN;
  constexpr static int k_maxNumberOfParameters = 20;

protected:
  const ParameterRepr * paramReprAtIndex(int i) const override { return nullptr; }
  float * paramArray() override { return m_input; }

  // Chi2 specific
  virtual float expectedValue(int index);
  virtual float observedValue(int index);

private:
  int _numberOfInputRows();
  static float _zAlpha(float degreesOfFreedom, float significanceLevel);
  static float _pVal(float degreesOfFreedom, float z);

  float m_input[k_maxNumberOfParameters];
  float m_degreesOfFreedom;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H */
