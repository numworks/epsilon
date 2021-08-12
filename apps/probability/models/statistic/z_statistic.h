#ifndef APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H

#include "probability/helpers.h"
#include "cached_statistic.h"

namespace Probability {

/* A t-Statistic is a Statistic whose distribution is a normal law. */
class ZStatistic : public CachedStatistic {
public:
  Poincare::Layout testCriticalValueSymbol() override { return layoutFromText("z"); }
  bool hasDegreeOfFreedom() override { return false; }
  float normalizedDensityFunction(float x) const override;
  float cumulativeNormalizedDistributionFunction(float x) const override;
  float cumulativeNormalizedInverseDistributionFunction(float proba) const override;

};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H */
