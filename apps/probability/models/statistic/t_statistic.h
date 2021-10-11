#ifndef PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H

#include <poincare/layout_helper.h>

#include "cached_statistic.h"
#include "probability/helpers.h"

namespace Probability {

/* A t-Statistic is a Statistic whose distribution is a Student law. */
class TStatistic : public CachedStatistic {
public:
  Poincare::Layout testCriticalValueSymbol() override {
    return Poincare::LayoutHelper::String("t");
  }
  bool hasDegreeOfFreedom() override { return true; }
  double degreeOfFreedom() override { return m_degreesOfFreedom; }
  float canonicalDensityFunction(float x) const override;
  double cumulativeNormalizedDistributionFunction(double x) const override;
  double cumulativeNormalizedInverseDistributionFunction(double proba) const override;

  float xMin() const override;
  float xMax() const override;
  float yMax() const override;

protected:
  double m_degreesOfFreedom;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H */
