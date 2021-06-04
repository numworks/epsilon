#ifndef APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_STATISTIC_H

#include "t_statistic.h"

namespace Probability {

class TwoMeansTStatistic : public TStatistic {
public:
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x1-x2"; };
  I18n::Message estimateDescription() override { return I18n::Message::Default; };

private:
  float _xEstimate(float meanSample1, float meanSample2);
  float _t(float deltaMean, float meanSample1, float n1, float sigma1, float meanSample2, float n2,
           float sigma2);
  float _degreeOfFreedom(float s1, int n1, float s2, int n2);
  float _SE(float s1, int n1, float s2, int n2);
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_TWO_MEANS_T_STATISTIC_H */
