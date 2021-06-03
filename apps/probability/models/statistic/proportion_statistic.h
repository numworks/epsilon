#ifndef APPS_PROBABILITY_MODELS_STATISTIC_PROPORTION_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_PROPORTION_STATISTIC_H

#include "statistic.h"

namespace Probability
{

class ProportionStatistic : public CachedStatistic {
protected:
  float _zCritical(float confidenceLevel);
  float _pVal(float z, char op);
  float _ME(float zCritical, float SE);
};


} // namespace Probability


#endif /* APPS_PROBABILITY_MODELS_STATISTIC_PROPORTION_STATISTIC_H */
