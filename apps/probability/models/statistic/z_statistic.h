#ifndef APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H

#include "probability/helpers.h"
#include "statistic.h"

namespace Probability {

/* A t-Statistic is a Statistic whose distribution is a normal law. */
class ZStatistic : public CachedStatistic {
public:
  Poincare::Layout testCriticalValueSymbol() override { return layoutFromText("z"); }
  bool hasDegreeOfFreedom() override { return false; }
  float normedDensityFunction(float x) override;

protected:
  float _zAlpha(float alpha);
  float _zCritical(float confidenceLevel);
  float _pVal(float z, char op);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H */
