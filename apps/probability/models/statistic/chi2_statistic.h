#ifndef APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H

#include <cmath>

#include "statistic.h"

namespace Probability {

class Chi2Statistic : public CachedStatistic {
public:
  void computeTest() override;
  const char * testCriticalValueSymbol() override { return "X2"; }
  const char * estimateSymbol() override { return ""; }
  I18n::Message estimateDescription() override { return I18n::Message::Default; }
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }

  float normedDensityFunction(float x) override;

  void computeInterval() override {}
  const char * intervalCriticalValueSymbol() override { return ""; }

  constexpr static float k_undefinedValue = NAN;

protected:
  const ParameterRepr * paramReprAtIndex(int i) const override { return nullptr; }

  // Chi2 specific
  virtual float expectedValue(int index) = 0;
  virtual float observedValue(int index) = 0;
  virtual int _degreesOfFreedom() = 0;
  virtual int numberOfValuePairs() = 0;

private:
  static float _zAlpha(float degreesOfFreedom, float significanceLevel);
  static float _pVal(float degreesOfFreedom, float z);
  float m_degreesOfFreedom;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H */
