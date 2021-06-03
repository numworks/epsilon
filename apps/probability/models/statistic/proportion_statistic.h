#ifndef APPS_PROBABILITY_MODELS_STATISTIC_PROPORTION_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_PROPORTION_STATISTIC_H

#include "statistic.h"

namespace Probability {

class ProportionStatistic : public CachedStatistic {
public:
  const char * testCriticalValueSymbol() override { return "z"; };
  const char * intervalCriticalValueSymbol() override { return "z*"; };
  bool hasDegreeOfFreedom() override { return false; };

protected:
  float _zCritical(float confidenceLevel);
  float _pVal(float z, char op);
  float _ME(float zCritical, float SE);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_PROPORTION_STATISTIC_H */
