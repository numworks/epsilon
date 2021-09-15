#ifndef PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H

#include <poincare/horizontal_layout.h>

#include <cmath>

#include "cached_statistic.h"

namespace Probability {

class Chi2Statistic : public CachedStatistic {
public:
  Chi2Statistic();
  Poincare::Layout testCriticalValueSymbol() override;
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }

  float canonicalDensityFunction(float x) const override;
  float cumulativeNormalizedDistributionFunction(float x) const override;
  float cumulativeNormalizedInverseDistributionFunction(float proba) const override;

  void computeInterval() override {}

  float xMin() const override;
  float xMax() const override;
  float yMax() const override;

  constexpr static float k_undefinedValue = NAN;

protected:
  using CachedStatistic::setParamAtIndex;  // Hidden
  using CachedStatistic::paramAtIndex;  // Hidden
  float m_degreesOfFreedom;

  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::HorizontalLayout::Builder(), I18n::Message::Default};
  }

  // Chi2 specific
  virtual float expectedValue(int index) = 0;
  virtual float observedValue(int index) = 0;
  virtual int numberOfValuePairs() = 0;

  float computeChi2();
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H */
