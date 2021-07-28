#ifndef APPS_PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H

#include "probability/helpers.h"
#include "statistic.h"

namespace Probability {

/* A t-Statistic is a Statistic whose distribution is a Student law. */
class TStatistic : public CachedStatistic {
public:
  Poincare::Layout testCriticalValueSymbol() override { return layoutFromText("t"); }
  bool hasDegreeOfFreedom() override { return true; }
  float degreeOfFreedom() override { return m_degreesOfFreedom; }
  float normedDensityFunction(float x) override;

protected:
  float _tAlpha(float degreesOfFreedom, float alpha);
  float _pVal(float degreesOfFreedom, float t);
  float _tCritical(float degreesOfFreedom, float confidenceLevel);
  float m_degreesOfFreedom;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_T_STATISTIC_H */
