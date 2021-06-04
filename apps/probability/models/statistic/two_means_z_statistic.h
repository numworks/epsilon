#ifndef APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class TwoMeansZStatistic : public ZStatistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x1-x2"; };
  I18n::Message estimateDescription() override { return I18n::Message::Default; };

protected:
  float _xEstimate(float meanSample1, float meanSample2);
  float _z(float deltaMean, float meanSample1, float n1, float sigma1, float meanSample2, float n2,
           float sigma2);
  float _SE(float sigma1, int n1, float sigma2, int n2);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_Z_STATISTIC_H */
