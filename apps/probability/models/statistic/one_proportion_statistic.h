#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H

#include "proportion_statistic.h"

namespace Probability
{

class OneProportionStatistic : public ProportionStatistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Test statistic
  const char * testCriticalValueSymbol() override { return "z"; };
  bool hasDegreeOfFreedom() override { return false; };

  // Confidence interval
  const char * estimateSymbol() override { return "p"; };
  I18n::Message estimateDescription() override { return I18n::Message::SampleProportion; };
  const char * intervalCriticalValueSymbol() override { return "z*"; };

private:
  float _pEstimate(float x, float n);
  float _z(float p0, float p, int n);
  float _SE(float pEstimate, int n);
};


} // namespace Probability


#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H */
