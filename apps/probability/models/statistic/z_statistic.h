#ifndef APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H

#include "statistic.h"

namespace Probability {

class ZStatistic : public CachedStatistic {
public:
  const char * testCriticalValueSymbol() override { return "z"; };
  const char * intervalCriticalValueSymbol() override { return "z*"; };
  float _zCritical(float confidenceLevel);
  bool hasDegreeOfFreedom() override { return false; }

protected:
  float _pVal(float z, char op);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_Z_STATISTIC_H */
