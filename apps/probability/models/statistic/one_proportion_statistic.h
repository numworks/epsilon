#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class OneProportionStatistic : public ZStatistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "p"; };
  I18n::Message estimateDescription() override { return I18n::Message::SampleProportion; };

private:
  float _pEstimate(float x, float n);
  float _z(float p0, float p, int n);
  float _SE(float pEstimate, int n);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_PROPORTION_STATISTIC_H */
