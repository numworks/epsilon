#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H

#include "t_statistic.h"

namespace Probability {

// TODO factor with OneMeanZStatistic
class OneMeanTStatistic : public TStatistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x"; };
  I18n::Message estimateDescription() override { return I18n::Message::Default; };

protected:
  float _degreesOfFreedom(int n);
  float _t(float mean, float meanSample, float s, float n);
  float _SE(float s, float n);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_STATISTIC_H */
