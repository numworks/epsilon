#ifndef APPS_PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H

#include "cached_statistic.h"
#include "probability/helpers.h"

namespace Probability {

/* A t-Statistic is a Statistic whose distribution is a Student law. */
class TStatistic : public CachedStatistic {
public:
  Poincare::Layout testCriticalValueSymbol() override { return layoutFromText("t"); }
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }
  float normalizedDensityFunction(float x) const override;
  float cumulativeNormalizedDistributionFunction(float x) const override;
  float cumulativeNormalizedInverseDistributionFunction(float proba) const override;

protected:
  float m_degreesOfFreedom;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H */
