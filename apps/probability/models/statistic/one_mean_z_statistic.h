#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class OneMeanZStatistic : public ZStatistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x"; };
  I18n::Message estimateDescription() override { return I18n::Message::Default; };

protected:
  float _z(float meanSample, float mean, float n, float sigma);
  float _SE(float sigma, int n);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H */
