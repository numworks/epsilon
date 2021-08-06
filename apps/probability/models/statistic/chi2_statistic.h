#ifndef APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H

#include <poincare/horizontal_layout.h>

#include <cmath>

#include "statistic.h"

namespace Probability {

class Chi2Statistic : public CachedStatistic {
public:
  Chi2Statistic();
  Poincare::Layout testCriticalValueSymbol() override;
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }

  float normedDensityFunction(float x) override;

  void computeInterval() override {}

  constexpr static float k_undefinedValue = NAN;

protected:
  float m_degreesOfFreedom;

  ParameterRepr paramReprAtIndex(int i) const override {
    return ParameterRepr{Poincare::HorizontalLayout::Builder(), I18n::Message::Default};
  }

  // Chi2 specific
  virtual float expectedValue(int index) = 0;
  virtual float observedValue(int index) = 0;
  virtual int numberOfValuePairs() = 0;

  float _z();
  static float _zAlpha(float degreesOfFreedom, float significanceLevel);
  static float _pVal(float degreesOfFreedom, float z);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H */
